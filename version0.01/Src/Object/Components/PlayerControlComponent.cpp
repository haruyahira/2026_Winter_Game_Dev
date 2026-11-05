#include "PlayerControlComponent.h"
#include <string>
#include <cctype>
#include "PhysicsComponent.h"
#include "AnimationComponent.h"
#include "MouseLookComponent.h"
#include "../Charactor/Player/ChantSystem.h"
#include "../Charactor/Player/Player.h"
#include "../Magic/GenericMagic.h"
#include "../Magic/ZoltraakMagic.h"
#include "../Magic/MagicData.h"
#include "../Magic/SystemMagic.h"
#include "../../Manager/InputManager.h"
#include "../../Manager/SceneManager.h"
#include "../../Manager/Camera.h"
#include "../../Utility/AsoUtility.h"

PlayerControlComponent::PlayerControlComponent(void)
	: chantSystem_(std::make_unique<ChantSystem>())
	, state_(STATE::NONE)
	, playerRotY_(Quaternion())
	, goalQuaRot_(Quaternion())
	, stepRotTime_(0.0f)
	, stepJump_(0.0f)
	, prevState_(STATE::NONE)
{
	stateChanges_.emplace(STATE::NONE, std::bind(&PlayerControlComponent::ChangeStateNone, this));
	stateChanges_.emplace(STATE::PLAY, std::bind(&PlayerControlComponent::ChangeStatePlay, this));
	stateChanges_.emplace(STATE::CHANT, std::bind(&PlayerControlComponent::ChangeStateChant, this));
	stateChanges_.emplace(STATE::FLY, std::bind(&PlayerControlComponent::ChangeStateFly, this));
}

PlayerControlComponent::~PlayerControlComponent(void)
{
}

void PlayerControlComponent::Init(void)
{
	ChangeState(STATE::PLAY);
}

void PlayerControlComponent::Update(void)
{
	if (stateUpdate_)
	{
		stateUpdate_();
	}
}

void PlayerControlComponent::Draw(void)
{
	if (chantSystem_)
	{
		chantSystem_->Draw();
	}
}

void PlayerControlComponent::ChangeState(STATE state)
{
	prevState_ = state_;
	state_ = state;
	stateChanges_[state_]();
}

void PlayerControlComponent::ChangeStateNone(void)
{
	stateUpdate_ = std::bind(&PlayerControlComponent::UpdateNone, this);
	if (owner_)
	{
		if (auto physics = owner_->GetComponent<PhysicsComponent>())
		{
			physics->SetGravityEnabled(false);
		}
	}
}

void PlayerControlComponent::ChangeStatePlay(void)
{
	stateUpdate_ = std::bind(&PlayerControlComponent::UpdatePlay, this);
	if (owner_)
	{
		if (auto physics = owner_->GetComponent<PhysicsComponent>())
		{
			physics->SetGravityEnabled(true);
		}
	}
}

void PlayerControlComponent::ChangeStateChant(void)
{
	stateUpdate_ = std::bind(&PlayerControlComponent::UpdateChant, this);

	if (owner_)
	{
		if (auto physics = owner_->GetComponent<PhysicsComponent>())
		{
			physics->movePow_ = AsoUtility::VECTOR_ZERO;
			physics->SetGravityEnabled(false);
		}
		if (auto anim = owner_->GetComponent<AnimationComponent>())
		{
			anim->Play((int)ANIM_TYPE::WARP_PAUSE);
		}
	}

	chantSystem_->SetHintText("【Enter/左ｸﾘｯｸ】発動 / 【Tab】取消\n攻撃: fire ball 等 / 再起: retry / 帰還: title");
	chantSystem_->Start();
}

void PlayerControlComponent::ChangeStateFly(void)
{
	stateUpdate_ = std::bind(&PlayerControlComponent::UpdateFly, this);
	if (owner_)
	{
		if (auto physics = owner_->GetComponent<PhysicsComponent>())
		{
			physics->movePow_ = AsoUtility::VECTOR_ZERO;
			physics->jumpPow_ = AsoUtility::VECTOR_ZERO;
			physics->SetGravityEnabled(false);
		}

		// ★ 修正：詠唱からの復帰時は上にワープさせない（無限に上昇してしまうのを防ぐ）
		if (prevState_ != STATE::CHANT)
		{
			owner_->transform_.pos.y += 5.0f;
		}
	}
	flyTime_ = 0.0f;
}

void PlayerControlComponent::UpdateNone(void)
{
}

void PlayerControlComponent::UpdatePlay(void)
{
	if (!owner_) return;

	if (InputManager::GetInstance().IsTrgDown(KEY_INPUT_Z))
	{
		ChangeState(STATE::FLY);
		return;
	}

	if (auto physics = owner_->GetComponent<PhysicsComponent>())
	{
		physics->SetGravityEnabled(true);

		// フリーカメラ時入力を無視
		if (!mainCamera.IsActive())
		{
			physics->movePow_ = AsoUtility::VECTOR_ZERO;
			return;
		}
	}

	Rotate();
	owner_->transform_.quaRot = playerRotY_;

	ProcessMove();
	ProcessJump();
	ProcessAttack();
	ProcessDefense();
}

void PlayerControlComponent::UpdateChant(void)
{
	if (!owner_) return;

	Rotate();
	owner_->transform_.quaRot = playerRotY_;

	auto& ins = InputManager::GetInstance();

	// ★ これを追加：戻るべき状態をチェック
	STATE returnState = (prevState_ == STATE::FLY) ? STATE::FLY : STATE::PLAY;

	if (ins.IsTrgDown(KEY_INPUT_TAB))
	{
		chantSystem_->Cancel();
		ChangeState(returnState);
		return;
	}

	chantSystem_->Update();

	if (chantSystem_->IsFinished())
	{
		std::string spell = chantSystem_->GetNormalizedSpell();

		if (SystemMagic::Execute(spell))
		{
			chantSystem_->Cancel();
			return;
		}

		if (MagicDatabase::IsValidSpell(spell))
		{
			if (Player* player = dynamic_cast<Player*>(owner_))
			{
				player->StartCasting(spell);
			}
		}

		chantSystem_->Cancel();
		ChangeState(returnState);
	}
}

void PlayerControlComponent::UpdateFly(void)
{
	if (!owner_) return;

	auto& ins = InputManager::GetInstance();

	if (ins.IsTrgDown(KEY_INPUT_Z))
	{
		ChangeState(STATE::PLAY);
		return;
	}

	if (!mainCamera.IsActive())
	{
		if (auto physics = owner_->GetComponent<PhysicsComponent>())
		{
			physics->movePow_ = AsoUtility::VECTOR_ZERO;
		}
		return; // ここで処理を終わらせるため、下の移動処理が呼ばれない
	}

	// 飛行中もTABキーで詠唱開始！
	if (ins.IsTrgDown(KEY_INPUT_TAB))
	{
		ChangeState(STATE::CHANT);
		return;
	}

	VECTOR dir = AsoUtility::VECTOR_ZERO;
	Quaternion cameraRot = mainCamera.GetQuaRot();

	if (ins.IsNew(KEY_INPUT_W)) { dir = VAdd(dir, cameraRot.GetForward()); }
	if (ins.IsNew(KEY_INPUT_S)) { dir = VAdd(dir, cameraRot.GetBack()); }
	if (ins.IsNew(KEY_INPUT_D)) { dir = VAdd(dir, cameraRot.GetRight()); }
	if (ins.IsNew(KEY_INPUT_A)) { dir = VAdd(dir, cameraRot.GetLeft()); }

	if (ins.IsNew(KEY_INPUT_SPACE)) { dir = VAdd(dir, AsoUtility::DIR_U); }
	if (ins.IsNew(KEY_INPUT_LCONTROL)) { dir = VAdd(dir, AsoUtility::DIR_D); }

	if (auto physics = owner_->GetComponent<PhysicsComponent>())
	{
		physics->jumpPow_ = AsoUtility::VECTOR_ZERO;
		physics->movePow_ = AsoUtility::VECTOR_ZERO;

		if (!AsoUtility::EqualsVZero(dir))
		{
			dir = AsoUtility::VNormalize(dir);
			float dtScale = SceneManager::GetInstance().GetDeltaTime() * 60.0f;

			// ★直接ワープするのではなく、物理エンジンに移動量(3D方向)を渡す
			physics->speed_ = Player::SPEED_RUN;
			physics->movePow_ = VScale(dir, physics->speed_ * dtScale);
		}
	}

	// プレイヤーの体の向き（とカプセル当たり判定）を進行方向に傾ける
	owner_->transform_.quaRot = cameraRot;

	flyTime_ += SceneManager::GetInstance().GetDeltaTime();
}

void PlayerControlComponent::ProcessMove(void)
{
	if (!owner_) return;
	auto physics = owner_->GetComponent<PhysicsComponent>();
	auto anim = owner_->GetComponent<AnimationComponent>();
	if (!physics || !anim) return;

	auto& ins = InputManager::GetInstance();
	physics->movePow_ = AsoUtility::VECTOR_ZERO;

	Quaternion cameraRot = mainCamera.GetQuaRotOutX();
	VECTOR dir = AsoUtility::VECTOR_ZERO;

	if (ins.IsNew(KEY_INPUT_W)) { dir = VAdd(dir, cameraRot.GetForward()); }
	if (ins.IsNew(KEY_INPUT_S)) { dir = VAdd(dir, cameraRot.GetBack()); }
	if (ins.IsNew(KEY_INPUT_D)) { dir = VAdd(dir, cameraRot.GetRight()); }
	if (ins.IsNew(KEY_INPUT_A)) { dir = VAdd(dir, cameraRot.GetLeft()); }

	if (!AsoUtility::EqualsVZero(dir) && (physics->isJump_ || IsEndLanding()))
	{
		dir = AsoUtility::VNormalize(dir);

		float dtScale = SceneManager::GetInstance().GetDeltaTime() * 60.0f;
		bool isRun = ins.IsNew(KEY_INPUT_RSHIFT) || ins.IsNew(KEY_INPUT_LSHIFT);

		physics->speed_ = isRun ? Player::SPEED_RUN : Player::SPEED_WALK;
		physics->moveDir_ = dir;
		physics->movePow_ = VScale(dir, physics->speed_ * dtScale);

		if (!physics->isJump_ && IsEndLanding())
		{
			anim->Play(isRun ? (int)ANIM_TYPE::FAST_RUN : (int)ANIM_TYPE::RUN);
		}
	}
	else
	{
		if (!physics->isJump_ && IsEndLanding())
		{
			anim->Play((int)ANIM_TYPE::IDLE);
		}
	}
}

void PlayerControlComponent::ProcessJump(void)
{
	if (!owner_) return;
	auto physics = owner_->GetComponent<PhysicsComponent>();
	auto anim = owner_->GetComponent<AnimationComponent>();
	if (!physics || !anim) return;

	auto& scnMng = SceneManager::GetInstance();
	bool isHit = CheckHitKey(KEY_INPUT_SPACE);

	if (isHit && (physics->isJump_ || IsEndLanding()))
	{
		if (!physics->isJump_)
		{
			anim->Play((int)ANIM_TYPE::JUMP, true, Player::ANIM_JUMP_START, Player::ANIM_JUMP_LOOP_END);
			anim->SetEndLoop(Player::ANIM_JUMP_LOOP_START, Player::ANIM_JUMP_LOOP_END, Player::ANIM_JUMP_BLEND);
		}
		physics->isJump_ = true;
		stepJump_ += scnMng.GetDeltaTime();
		if (stepJump_ < Player::TIME_JUMP_IN)
		{
			physics->jumpPow_ = VScale(AsoUtility::DIR_U, Player::POW_JUMP);
		}
	}

	if (!isHit)
	{
		stepJump_ = Player::TIME_JUMP_IN;
	}
}

void PlayerControlComponent::ProcessDefense(void)
{
}

void PlayerControlComponent::ProcessAttack(void)
{
	auto& ins = InputManager::GetInstance();
	float deltaTime = SceneManager::GetInstance().GetDeltaTime();

	if (stepRotTime_ > 0.0f)
	{
		stepRotTime_ -= deltaTime;
	}

	if (ins.IsTrgDown(KEY_INPUT_TAB))
	{
		ChangeState(STATE::CHANT);
	}
}

void PlayerControlComponent::ProcessMagic(const std::string& spellName)
{
	if (!owner_) return;
	if (!MagicDatabase::IsValidSpell(spellName)) return;

	// 先に魔法データを取得する
	MagicData data = MagicDatabase::GetData(spellName);

	// MagicCircleComponent(魔法陣)と全く同じ計算式にしてズレを無くす！
	VECTOR cPos = mainCamera.GetPos();
	VECTOR cTarget = mainCamera.GetTargetPos();
	VECTOR cDir = VNorm(VSub(cTarget, cPos));

	Transform spawnTrans = owner_->transform_;
	spawnTrans.quaRot = playerRotY_;

	spawnTrans.pos = VAdd(cPos, VScale(cDir, data.spawnDistance));
	spawnTrans.pos.y -= 50.0f;

	std::unique_ptr<GenericMagic> magic;

	if (data.name == "zoltraak")
	{
		magic = std::make_unique<ZoltraakMagic>(data);
	}
	else
	{
		magic = std::make_unique<GenericMagic>(data);
	}

	magic->Init();
	magic->Fire(spawnTrans, cDir);

	if (spawnActorFunc_)
	{
		spawnActorFunc_(std::move(magic));
	}
}

void PlayerControlComponent::SetGoalRotate(double rotRad)
{
	VECTOR cameraRot = mainCamera.GetAngles();
	goalQuaRot_ = Quaternion::AngleAxis((double)cameraRot.y + rotRad, AsoUtility::AXIS_Y);
}

void PlayerControlComponent::Rotate(void)
{
	playerRotY_ = mainCamera.GetQuaRotOutX();
	goalQuaRot_ = playerRotY_;
}

void PlayerControlComponent::OnLanding(void)
{
	if (!owner_) return;
	auto physics = owner_->GetComponent<PhysicsComponent>();
	auto anim = owner_->GetComponent<AnimationComponent>();

	stepJump_ = 0.0f;
	if (physics && physics->isJump_)
	{
		if (anim)
		{
			anim->Play((int)ANIM_TYPE::JUMP, false, Player::ANIM_LANDING_START, Player::ANIM_LANDING_END, false, true);
		}
	}
	if (physics)
	{
		physics->isJump_ = false;
	}
}

bool PlayerControlComponent::IsEndLanding(void)
{
	if (!owner_) return true;
	auto anim = owner_->GetComponent<AnimationComponent>();
	if (!anim) return true;
	if (anim->GetPlayType() != (int)ANIM_TYPE::JUMP) return true;
	if (anim->IsEnd()) return true;
	return false;
}
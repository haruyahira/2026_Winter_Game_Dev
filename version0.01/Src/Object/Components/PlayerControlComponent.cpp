// Src/Object/Components/PlayerControlComponent.cpp
#include "PlayerControlComponent.h"
#include <string>
#include "PhysicsComponent.h"
#include "AnimationComponent.h"
#include "../Charactor/Player/ChantSystem.h"
#include "../Magic/FireBall.h"
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
{
	stateChanges_.emplace(STATE::NONE, std::bind(&PlayerControlComponent::ChangeStateNone, this));
	stateChanges_.emplace(STATE::PLAY, std::bind(&PlayerControlComponent::ChangeStatePlay, this));
	stateChanges_.emplace(STATE::CHANT, std::bind(&PlayerControlComponent::ChangeStateChant, this));
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

	chantSystem_->Start();
}

void PlayerControlComponent::UpdateNone(void)
{
}

void PlayerControlComponent::UpdatePlay(void)
{
	if (!owner_) return;
	if (auto physics = owner_->GetComponent<PhysicsComponent>())
	{
		physics->SetGravityEnabled(true);
	}

	// ★ 1. まずプレイヤーの向きをカメラの水平向き（マウス回転）と完全に一致させる！
	Rotate();
	owner_->transform_.quaRot = playerRotY_;

	// ★ 2. その向きを基準にして移動・ジャンプ・攻撃を行う
	ProcessMove();
	ProcessJump();
	ProcessAttack();
	ProcessDefense();
}

void PlayerControlComponent::UpdateChant(void)
{
	if (!owner_) return;

	// ★ 詠唱中もマウスを動かせばカメラと一緒に好きな方向へ狙いを定められる！
	Rotate();
	owner_->transform_.quaRot = playerRotY_;

	auto& ins = InputManager::GetInstance();

	if (ins.IsTrgDown(KEY_INPUT_TAB))
	{
		chantSystem_->Cancel();
		ChangeState(STATE::PLAY);
		return;
	}

	chantSystem_->Update();

	if (chantSystem_->IsFinished())
	{
		std::string spell = chantSystem_->GetTypedText();

		if (spell == "fire ball")
		{
			ProcessMagic();
		}

		chantSystem_->Cancel();
		ChangeState(STATE::PLAY);
	}
}

void PlayerControlComponent::ProcessMove(void)
{
	if (!owner_) return;
	auto physics = owner_->GetComponent<PhysicsComponent>();
	auto anim = owner_->GetComponent<AnimationComponent>();
	if (!physics || !anim) return;

	auto& ins = InputManager::GetInstance();
	physics->movePow_ = AsoUtility::VECTOR_ZERO;

	// カメラ（＝プレイヤー）の向きを基準に前後左右の移動ベクトルを合成
	Quaternion cameraRot = mainCamera.GetQuaRotOutX();
	VECTOR dir = AsoUtility::VECTOR_ZERO;

	if (ins.IsNew(KEY_INPUT_W)) { dir = VAdd(dir, cameraRot.GetForward()); }
	if (ins.IsNew(KEY_INPUT_S)) { dir = VAdd(dir, cameraRot.GetBack()); }
	if (ins.IsNew(KEY_INPUT_D)) { dir = VAdd(dir, cameraRot.GetRight()); }
	if (ins.IsNew(KEY_INPUT_A)) { dir = VAdd(dir, cameraRot.GetLeft()); }

	if (!AsoUtility::EqualsVZero(dir) && (physics->isJump_ || IsEndLanding()))
	{
		dir = AsoUtility::VNormalize(dir);

		// ★ 60FPS基準のデルタタイム補正（どのPCでも完全に同じ移動速度になる）
		float dtScale = SceneManager::GetInstance().GetDeltaTime() * 60.0f;
		bool isRun = ins.IsNew(KEY_INPUT_RSHIFT) || ins.IsNew(KEY_INPUT_LSHIFT);

		physics->speed_ = isRun ? Player::SPEED_RUN : Player::SPEED_MOVE;
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

	// 攻撃クールタイムの更新（FPS非依存：秒単位で管理）
	if (stepRotTime_ > 0.0f)
	{
		stepRotTime_ -= deltaTime;
	}

	// 左クリックまたはEnterキーでクールタイム（0.3秒）ごとに魔法発射も可能
	if ((ins.IsTrgMouseLeft() || CheckHitKey(KEY_INPUT_RETURN)) && stepRotTime_ <= 0.0f)
	{
		ProcessMagic();
		stepRotTime_ = 0.3f;
	}

	// TABキーで「詠唱モード」へ移行！
	if (ins.IsTrgDown(KEY_INPUT_TAB))
	{
		ChangeState(STATE::CHANT);
	}
}

void PlayerControlComponent::ProcessMagic(void)
{
	if (!owner_) return;

	// ★ カメラ（＝プレイヤー）が向いている正面方向へ魔法を発射！
	VECTOR forward = playerRotY_.GetForward();

	Transform spawnTrans = owner_->transform_;
	spawnTrans.pos = VAdd(spawnTrans.pos, VGet(0.0f, 80.0f, 0.0f));
	spawnTrans.pos = VAdd(spawnTrans.pos, VScale(forward, 30.0f));

	auto magic = std::make_unique<FireBall>();
	magic->Init();
	magic->Fire(spawnTrans, forward);

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
	// ★ カメラのY軸回転とプレイヤーのY軸回転を常に一致させる（360度マウス連動）
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
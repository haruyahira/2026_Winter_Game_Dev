#include "BossEnemy.h"

#include "../../../Manager/ResourceManager.h"
#include "../../../Manager/SceneManager.h"
#include "../../../Utility/AsoUtility.h"
#include "../../Common/AnimationController.h"
#include "../../Common/Capsule.h"
#include "../../Common/Collider.h"
#include "../../Planet.h"

BossEnemy::BossEnemy(void)
{
	state_ = STATE::NONE;
	step_ = 0.0f;
}

BossEnemy::~BossEnemy(void)
{
}

void BossEnemy::InitDetail(void)
{
	vsName_ = "ToonModelVS2.cso";
	psName_ = "ToonModelPS2.cso";

	auto& resMng = ResourceManager::GetInstance();

	// モデル(リソースID, スケール, 座標, ローカル軸回転)
	LoadAndSetupModel(
		ResourceManager::SRC::BOSS_ENEMY,
		INIT_SCALE,
		INIT_POSITION,
		INIT_AXIS);

	// 髪と槍を非表示にする
	MV1SetFrameVisible(transform_.modelId, 3, false);
	//MV1SetFrameVisible(transform_.modelId, 132, false);

	// 当たり判定(上端Y, 下端Y, 半径)
	SetupCapsule(CAPSULE_TOP, CAPSULE_BOTTOM, CAPSULE_RADIUS);

	// アニメーションの準備
	InitAnimation();

	// 初期状態
	ChangeState(STATE::MOVE);
}

void BossEnemy::InitAnimation(void)
{
	// アニメーションの設定
	animationController_ = std::make_unique<AnimationController>(transform_.modelId);
	animationController_->AddInFBX((int)ANIM_TYPE::IDLE, 30.0f);
	animationController_->AddInFBX((int)ANIM_TYPE::WALK, 30.0f);

	animationController_->Play((int)ANIM_TYPE::IDLE);
}

// 毎フレーム EnemyBase::Update から呼び出される AI処理
void BossEnemy::ProcessAI(void)
{
	switch (state_)
	{
	case STATE::NONE:
		break;
	case STATE::THINK:
		UpdateThink();
		break;
	case STATE::IDLE:
		UpdateIdle();
		break;
	case STATE::MOVE:
		UpdateMove();
		break;
	case STATE::ACTION:
		UpdateAction();
		break;
	case STATE::END:
		break;
	default:
		break;
	}
}

void BossEnemy::ChangeState(STATE state)
{
	state_ = state;
	step_ = 0.0f;
	movePow_ = AsoUtility::VECTOR_ZERO; // 親クラス(CharactorBase)の変数

	switch (state_)
	{
	case STATE::NONE:
		break;
	case STATE::THINK:
	{
		// 2(IDLE) か 3(MOVE) をランダムで選ぶ
		int tmpState = GetRand(1) + 2;
		ChangeState((STATE)tmpState);
	}
		break;
	case STATE::IDLE:
		animationController_->Play((int)ANIM_TYPE::IDLE);
		step_ = (float)(2000 + GetRand(3000)) / 1000.0f;
		break;
	case STATE::MOVE:
	{
		animationController_->Play((int)ANIM_TYPE::WALK);
		step_ = (float)(3000 + GetRand(3000)) / 1000.0f;

		// 移動方向をランダムに決定
		moveDir_ = VECTOR();
		moveDir_.x = ((float)(GetRand(2000)) / 1000.0f) - 1.0f;
		moveDir_.y = 0.0f;
		moveDir_.z = ((float)(GetRand(2000)) / 1000.0f) - 1.0f;
		moveDir_ = VNorm(moveDir_);
	}
		break;
	case STATE::ACTION:
		break;
	case STATE::END:
		break;
	default:
		break;
	}
}

void BossEnemy::UpdateThink(void)
{
}

void BossEnemy::UpdateIdle(void)
{
	step_ -= SceneManager::GetInstance().GetDeltaTime();
	if (step_ < 0.0f)
	{
		ChangeState(STATE::THINK);
	}
}

void BossEnemy::UpdateMove(void)
{
	// movePow_ に値を入れるだけで、実際の移動と壁との衝突処理は EnemyBase がやってくれる
	movePow_ = VScale(moveDir_, 2.0f);

	// --- 崖・壁を予測するAI思考 ---
	auto testPos = VAdd(transform_.pos, VScale(movePow_, 5.0f));
	VECTOR dirGravity = AsoUtility::DIR_D;
	VECTOR dirUpGravity = AsoUtility::DIR_U;
	float gravityPow = Planet::DEFAULT_GRAVITY_POW;
	float checkPow = 10.0f;

	auto gravHitPosUp = VAdd(testPos, VScale(dirUpGravity, gravityPow));
	gravHitPosUp = VAdd(gravHitPosUp, VScale(dirUpGravity, checkPow * 2.0f));
	auto gravHitPosDown = VAdd(testPos, VScale(dirGravity, checkPow * 10.0f));

	for (const auto c : colliders_)
	{
		auto hit = MV1CollCheck_Line(c.lock()->modelId_, -1, gravHitPosUp, gravHitPosDown);

		// もし目の前に地面が無い（崖）なら、Uターンする
		if (hit.HitFlag == 0)
		{
			moveDir_ = VScale(moveDir_, -1.0f);
			movePow_ = VScale(moveDir_, 4.0f);
		}
	}

	// 進行方向へ滑らかに回転
	transform_.quaRot = Quaternion::Slerp(transform_.quaRot, Quaternion::LookRotation(moveDir_), 0.1f);

	// 時間経過で思考へ戻る
	step_ -= SceneManager::GetInstance().GetDeltaTime();
	if (step_ < 0.0f)
	{
		ChangeState(STATE::THINK);
	}
}

void BossEnemy::UpdateAction(void)
{
	step_ -= SceneManager::GetInstance().GetDeltaTime();
	if (step_ < 0.0f)
	{
		ChangeState(STATE::THINK);
	}
}


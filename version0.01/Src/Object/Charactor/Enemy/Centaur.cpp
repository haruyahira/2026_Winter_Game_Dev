#include "Centaur.h"
#include "../../../Manager/ResourceManager.h"
#include "../../../Manager/SceneManager.h"
#include "../../../Utility/AsoUtility.h"
#include "../../Common/AnimationController.h"
#include "../../Common/Capsule.h"
#include "../../Common/Collider.h"
#include "../../Planet.h"

Centaur::Centaur(void)
{
	cState_ = CENTAUR_STATE::NONE;
	step_ = 0.0f;
}

Centaur::~Centaur(void)
{
}

void Centaur::InitDetail(void)
{
	vsName_ = "ToonModelVS2.cso";
	psName_ = "ToonModelPS2.cso";

	auto& resMng = ResourceManager::GetInstance();

	// モデルの基本設定
	transform_.SetModel(resMng.LoadModelDuplicate(ResourceManager::SRC::CENTAUR));
	transform_.scl = VScale(AsoUtility::VECTOR_ONE, 1.5f);
	transform_.pos = { 150.0f, -40.0f, 200.0f };
	transform_.quaRot = Quaternion();
	transform_.quaRotLocal = Quaternion::Euler({ 0.0f, AsoUtility::Deg2RadF(300.0f), 0.0f });
	transform_.Update();

	// 髪と槍を非表示にする
	MV1SetFrameVisible(transform_.modelId, 3, false);
	MV1SetFrameVisible(transform_.modelId, 132, false);

	// 2. アニメーションの設定
	animationController_ = std::make_unique<AnimationController>(transform_.modelId);
	animationController_->AddInFBX((int)ANIM_TYPE::IDLE, 30.0f);
	animationController_->AddInFBX((int)ANIM_TYPE::WALK, 30.0f);

	// 初期アニメーションを再生しないとモデルが表示されない場合があります
	animationController_->Play((int)ANIM_TYPE::IDLE);

	// 3. カプセルコライダ（当たり判定）のサイズ設定
	capsule_ = std::make_unique<Capsule>(transform_);
	capsule_->SetLocalPosTop({ 0.0f, 110.0f, 0.0f });
	capsule_->SetLocalPosDown({ 0.0f, 30.0f, 0.0f });
	capsule_->SetRadius(20.0f);

	// 初期状態へ
	ChangeState(CENTAUR_STATE::MOVE);
}

// 毎フレーム EnemyBase::Update から呼び出される AI処理
void Centaur::ProcessAI(void)
{
	switch (cState_)
	{
	case CENTAUR_STATE::NONE:
		break;
	case CENTAUR_STATE::THINK:
		UpdateThink();
		break;
	case CENTAUR_STATE::IDLE:
		UpdateIdle();
		break;
	case CENTAUR_STATE::MOVE:
		UpdateMove();
		break;
	case CENTAUR_STATE::ACTION:
		UpdateAction();
		break;
	case CENTAUR_STATE::END:
		break;
	default:
		break;
	}
}

void Centaur::ChangeState(CENTAUR_STATE state)
{
	cState_ = state;
	step_ = 0.0f;
	movePow_ = AsoUtility::VECTOR_ZERO; // 親クラス(CharactorBase)の変数

	switch (cState_)
	{
	case CENTAUR_STATE::NONE:
		break;
	case CENTAUR_STATE::THINK:
	{
		// 2(IDLE) か 3(MOVE) をランダムで選ぶ
		int tmpState = GetRand(2) + 2;
		ChangeState((CENTAUR_STATE)tmpState);
	}
	break;
	case CENTAUR_STATE::IDLE:
		animationController_->Play((int)ANIM_TYPE::IDLE);
		step_ = (float)(2000 + GetRand(3000)) / 1000.0f;
		break;
	case CENTAUR_STATE::MOVE:
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
	case CENTAUR_STATE::ACTION:
		break;
	case CENTAUR_STATE::END:
		break;
	default:
		break;
	}
}

void Centaur::UpdateThink(void)
{
}

void Centaur::UpdateIdle(void)
{
	step_ -= SceneManager::GetInstance().GetDeltaTime();
	if (step_ < 0.0f)
	{
		ChangeState(CENTAUR_STATE::THINK);
	}
}

void Centaur::UpdateMove(void)
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
		ChangeState(CENTAUR_STATE::THINK);
	}
}

void Centaur::UpdateAction(void)
{
	step_ -= SceneManager::GetInstance().GetDeltaTime();
	if (step_ < 0.0f)
	{
		ChangeState(CENTAUR_STATE::THINK);
	}
}


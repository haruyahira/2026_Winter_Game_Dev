// BossEnemy.cpp
#include "BossEnemy.h"
#include "../../../Manager/ResourceManager.h"
<<<<<<< HEAD
#include "../../Components/AnimationComponent.h"
#include "../../Components/CapsuleColliderComponent.h"
#include "../../Components/BossAIComponent.h"
#include "../../Components/StatusComponent.h"
=======
#include "../../Common/AnimationController.h"
#include "../../Common/Capsule.h"
#include "../../Components/BossAIComponent.h"
>>>>>>> f9ee5eae87468ea5e2cf6eeaa42714623e2e46bd

BossEnemy::BossEnemy(void)
{
}

BossEnemy::~BossEnemy(void)
{
}

void BossEnemy::InitDetail(void)
{
<<<<<<< HEAD
	SetShader("ToonModelVS2.cso", "ToonModelPS2.cso");
=======
	vsName_ = "ToonModelVS2.cso";
	psName_ = "ToonModelPS2.cso";
>>>>>>> f9ee5eae87468ea5e2cf6eeaa42714623e2e46bd

	// モデル(リソースID, スケール, 座標, ローカル軸回転)
	LoadAndSetupModel(
		ResourceManager::SRC::BOSS_ENEMY,
		INIT_SCALE,
		INIT_POSITION,
		INIT_AXIS);

	// 髪と槍を非表示にする
<<<<<<< HEAD
	HideModelFrame(3);
	//HideModelFrame(132);
=======
	MV1SetFrameVisible(transform_.modelId, 3, false);
>>>>>>> f9ee5eae87468ea5e2cf6eeaa42714623e2e46bd

	// 当たり判定(上端Y, 下端Y, 半径)とデバッグカプセル表示(青)
	SetupCapsule(CAPSULE_TOP, CAPSULE_BOTTOM, CAPSULE_RADIUS);
	if (capsuleComp_)
	{
		capsuleComp_->SetDrawDebug(true, GetColor(0, 0, 255));
	}

	// アニメーションの準備
	InitAnimation();

<<<<<<< HEAD
	// HP管理コンポーネントを取り付ける
	AddComponent(std::make_shared<StatusComponent>(MAX_HP, 160.0f));

	// ボスAIコンポーネントを取り付ける
=======
	// ★ AI部品を取り付ける！
>>>>>>> f9ee5eae87468ea5e2cf6eeaa42714623e2e46bd
	AddComponent(std::make_shared<BossAIComponent>());
}

void BossEnemy::InitAnimation(void)
{
<<<<<<< HEAD
	if (!animComp_) return;
	animComp_->AddInFBX((int)ANIM_TYPE::IDLE, 30.0f);
	animComp_->AddInFBX((int)ANIM_TYPE::WALK, 30.0f);
	animComp_->Play((int)ANIM_TYPE::IDLE);
=======
	animationController_ = std::make_unique<AnimationController>(transform_.modelId);
	animationController_->AddInFBX((int)ANIM_TYPE::IDLE, 30.0f);
	animationController_->AddInFBX((int)ANIM_TYPE::WALK, 30.0f);

	animationController_->Play((int)ANIM_TYPE::IDLE);
}

void BossEnemy::ProcessAI(void)
{
	// 自分が持っている全部品(BossAIComponentなど)のUpdate()を呼ぶ
	UpdateComponents();
>>>>>>> f9ee5eae87468ea5e2cf6eeaa42714623e2e46bd
}
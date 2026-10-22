#include "BossEnemy.h"
#include "../../../Manager/ResourceManager.h"
#include "../../Common/AnimationController.h"
#include "../../Common/Capsule.h"
#include "../../Components/BossAIComponent.h"

BossEnemy::BossEnemy(void)
{
}

BossEnemy::~BossEnemy(void)
{
}

void BossEnemy::InitDetail(void)
{
	vsName_ = "ToonModelVS2.cso";
	psName_ = "ToonModelPS2.cso";

	// モデル(リソースID, スケール, 座標, ローカル軸回転)
	LoadAndSetupModel(
		ResourceManager::SRC::BOSS_ENEMY,
		INIT_SCALE,
		INIT_POSITION,
		INIT_AXIS);

	// 髪と槍を非表示にする
	MV1SetFrameVisible(transform_.modelId, 3, false);

	// 当たり判定(上端Y, 下端Y, 半径)
	SetupCapsule(CAPSULE_TOP, CAPSULE_BOTTOM, CAPSULE_RADIUS);

	// アニメーションの準備
	InitAnimation();

	// ★ AI部品を取り付ける！
	AddComponent(std::make_shared<BossAIComponent>());
}

void BossEnemy::InitAnimation(void)
{
	animationController_ = std::make_unique<AnimationController>(transform_.modelId);
	animationController_->AddInFBX((int)ANIM_TYPE::IDLE, 30.0f);
	animationController_->AddInFBX((int)ANIM_TYPE::WALK, 30.0f);

	animationController_->Play((int)ANIM_TYPE::IDLE);
}

void BossEnemy::ProcessAI(void)
{
	// 自分が持っている全部品(BossAIComponentなど)のUpdate()を呼ぶ
	UpdateComponents();
}
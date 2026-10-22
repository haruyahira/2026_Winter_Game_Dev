#include "BossEnemy.h"
#include "../../../Manager/ResourceManager.h"
#include "../../Components/AnimationComponent.h"
#include "../../Components/CapsuleColliderComponent.h"
#include "../../Components/BossAIComponent.h"

BossEnemy::BossEnemy(void)
{
}

BossEnemy::~BossEnemy(void)
{
}

void BossEnemy::InitDetail(void)
{
	SetShader("ToonModelVS2.cso", "ToonModelPS2.cso");

	// モデル(リソースID, スケール, 座標, ローカル軸回転)
	LoadAndSetupModel(
		ResourceManager::SRC::BOSS_ENEMY,
		INIT_SCALE,
		INIT_POSITION,
		INIT_AXIS);

	// 髪と槍を非表示にする
	HideModelFrame(3);
	//HideModelFrame(132);

	// 当たり判定(上端Y, 下端Y, 半径)とデバッグカプセル表示(青)
	SetupCapsule(CAPSULE_TOP, CAPSULE_BOTTOM, CAPSULE_RADIUS);
	if (capsuleComp_)
	{
		capsuleComp_->SetDrawDebug(true, GetColor(0, 0, 255));
	}

	// アニメーションの準備
	InitAnimation();

	// ボスAIコンポーネントを取り付ける
	AddComponent(std::make_shared<BossAIComponent>());
}

void BossEnemy::InitAnimation(void)
{
	if (!animComp_) return;
	animComp_->AddInFBX((int)ANIM_TYPE::IDLE, 30.0f);
	animComp_->AddInFBX((int)ANIM_TYPE::WALK, 30.0f);
	animComp_->Play((int)ANIM_TYPE::IDLE);
}

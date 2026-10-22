// Destroyer.cpp
#include "Destroyer.h"
#include "../Manager/ResourceManager.h"
#include "Components/AnimationComponent.h"
#include "Components/CapsuleColliderComponent.h"
#include "Components/WanderAIComponent.h"
#include "Components/StatusComponent.h"

Destroyer::Destroyer(void)
{
}

Destroyer::~Destroyer(void)
{
}

void Destroyer::InitDetail(void)
{
	SetShader("ToonModelVS2.cso", "ToonModelPS2.cso");
	shadowSize_ = SHADOW_SIZE;

	// モデルの基本設定
	LoadAndSetupModel(
		ResourceManager::SRC::DESTROYER,
		INIT_SCALE,
		DEFAULT_POS,
		{ 0.0f, DEFAULT_ROT_Y_DEG, 0.0f });

	// カプセル当たり判定設定
	SetupCapsule(CAPSULE_TOP, CAPSULE_BOTTOM, CAPSULE_RADIUS);
	if (capsuleComp_)
	{
		capsuleComp_->SetDrawDebug(false);
	}

	// アニメーション設定
	InitAnimation();

	// HP管理コンポーネントを取り付ける
	AddComponent(std::make_shared<StatusComponent>(MAX_HP, 130.0f));

	// 徘徊AIコンポーネントを取り付ける（IDLE=3, RUN=10, 速度=4.0f）
	AddComponent(std::make_shared<WanderAIComponent>(
		(int)ANIM_TYPE::IDLE,
		(int)ANIM_TYPE::RUN,
		4.0f,
		2));
}

void Destroyer::InitAnimation(void)
{
	if (!animComp_) return;
	animComp_->AddInFBX((int)ANIM_TYPE::IDLE, 30.0f);
	animComp_->AddInFBX((int)ANIM_TYPE::RUN, 30.0f);
	animComp_->Play((int)ANIM_TYPE::IDLE);
}
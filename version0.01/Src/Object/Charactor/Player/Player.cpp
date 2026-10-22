// Src/Object/Charactor/Player/Player.cpp
#include "Player.h"
#include <string>
#include "../../../Application.h"
#include "../../../Manager/ResourceManager.h"
#include "../../Components/AnimationComponent.h"
#include "../../Components/PlayerControlComponent.h"

Player::Player(void)
	: controlComp_(nullptr)
{
}

Player::~Player(void)
{
}

void Player::InitDetail(void)
{
	// モデル(リソースID, スケール, 座標, ローカル軸回転)
	LoadAndSetupModel(
		ResourceManager::SRC::PLAYER,
		INIT_SCALE,
		INIT_POSITION,
		INIT_AXIS);

	// 当たり判定(上端Y, 下端Y, 半径)
	SetupCapsule(CAPSULE_TOP, CAPSULE_BOTTOM, CAPSULE_RADIUS);

	// アニメーションの準備
	InitAnimation();

	// プレイヤー操作・詠唱コンポーネントを取り付ける
	controlComp_ = std::make_shared<PlayerControlComponent>();
	if (spawnActorFunc_)
	{
		controlComp_->SetSpawnFunc(spawnActorFunc_);
	}
	AddComponent(controlComp_);
}

void Player::InitAnimation(void)
{
	if (!animComp_) return;
	std::string path = Application::PATH_MODEL + "Player/";
	animComp_->Add((int)ANIM_TYPE::IDLE, path + "Idle.mv1", 20.0f);
	animComp_->Add((int)ANIM_TYPE::RUN, path + "Run.mv1", 20.0f);
	animComp_->Add((int)ANIM_TYPE::FAST_RUN, path + "FastRun.mv1", 20.0f);
	animComp_->Add((int)ANIM_TYPE::JUMP, path + "Jump.mv1", 60.0f);
	animComp_->Add((int)ANIM_TYPE::WARP_PAUSE, path + "WarpPose.mv1", 60.0f);
	animComp_->Add((int)ANIM_TYPE::FLY, path + "Flying.mv1", 60.0f);
	animComp_->Add((int)ANIM_TYPE::FALLING, path + "Falling.mv1", 80.0f);
	animComp_->Add((int)ANIM_TYPE::VICTORY, path + "Victory.mv1", 60.0f);

	animComp_->Play((int)ANIM_TYPE::IDLE);
}

void Player::SetSpawnFunc(std::function<void(std::unique_ptr<ActorBase>)> func)
{
	spawnActorFunc_ = func;
	if (controlComp_)
	{
		controlComp_->SetSpawnFunc(func);
	}
}

void Player::OnLanding(void)
{
	if (controlComp_)
	{
		controlComp_->OnLanding();
	}
}
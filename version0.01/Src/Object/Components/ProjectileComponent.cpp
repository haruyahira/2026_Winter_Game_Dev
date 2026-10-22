// Src/Object/Components/ProjectileComponent.cpp
#include "ProjectileComponent.h"
#include "../ActorBase.h"
#include "../../Manager/SceneManager.h"
#include "../../Utility/AsoUtility.h"

ProjectileComponent::ProjectileComponent(float speed, float lifeTime)
	: moveDir_(AsoUtility::VECTOR_ZERO)
	, speed_(speed)
	, lifeTimer_(lifeTime)
	, onExpire_(nullptr)
{
}

void ProjectileComponent::Fire(VECTOR dir)
{
	if (!owner_) return;
	moveDir_ = VNorm(dir);
	owner_->transform_.quaRot = Quaternion::LookRotation(moveDir_);
	owner_->transform_.Update();
}

void ProjectileComponent::Update(void)
{
	if (!owner_) return;

	// ★ デルタタイムを用いて弾速と寿命を完全にFPS非依存化
	float deltaTime = SceneManager::GetInstance().GetDeltaTime();
	float dtScale = deltaTime * 60.0f;

	VECTOR movePow = VScale(moveDir_, speed_ * dtScale);
	owner_->transform_.pos = VAdd(owner_->transform_.pos, movePow);
	owner_->transform_.Update();

	lifeTimer_ -= deltaTime;
	if (lifeTimer_ <= 0.0f)
	{
		if (onExpire_)
		{
			onExpire_();
		}
	}
}
// EnemyBase.cpp
#include "EnemyBase.h"
#include "../../Components/PhysicsComponent.h"
#include "../../Components/StatusComponent.h"

EnemyBase::EnemyBase(void)
	: state_(STATE::IDLE)
{
}

EnemyBase::~EnemyBase(void)
{
}

void EnemyBase::Update(void)
{
	if (isDestroy()) return;

	ProcessAI();
	UpdateComponents();
}

void EnemyBase::Draw(void)
{
	if (isDestroy()) return;

	DrawComponents();
}

bool EnemyBase::isDestroy(void)
{
	if (auto status = GetComponent<StatusComponent>())
	{
		return !status->IsAlive();
	}
	return false;
}

void EnemyBase::SetTarget(std::weak_ptr<Player> target)
{
	target_ = target;
}

void EnemyBase::ProcessAI(void)
{
}

void EnemyBase::OnLanding(void)
{
	if (physicsComp_)
	{
		physicsComp_->isJump_ = false;
	}
}
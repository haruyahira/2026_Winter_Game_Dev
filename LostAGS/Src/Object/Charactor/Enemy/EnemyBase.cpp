#include "EnemyBase.h"
#include "../../Components/PhysicsComponent.h"

EnemyBase::EnemyBase(void)
	: state_(STATE::IDLE)
{
}

EnemyBase::~EnemyBase(void)
{
}

void EnemyBase::Update(void)
{
	ProcessAI();
	UpdateComponents();
}

void EnemyBase::Draw(void)
{
	DrawComponents();
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

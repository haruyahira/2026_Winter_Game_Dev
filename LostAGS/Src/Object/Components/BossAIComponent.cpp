#include "BossAIComponent.h"
#include "../Charactor/Enemy/BossEnemy.h"

BossAIComponent::BossAIComponent(void)
	: WanderAIComponent(
		(int)BossEnemy::ANIM_TYPE::IDLE,
		(int)BossEnemy::ANIM_TYPE::WALK,
		2.0f,
		1)
{
}

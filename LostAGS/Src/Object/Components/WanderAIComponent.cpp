// Src/Object/Components/WanderAIComponent.cpp
#include "WanderAIComponent.h"
#include "PhysicsComponent.h"
#include "AnimationComponent.h"
#include "CapsuleColliderComponent.h"
#include "../ActorBase.h"
#include "../Common/Collider.h"
#include "../Planet.h"
#include "../../Manager/SceneManager.h"
#include "../../Utility/AsoUtility.h"

WanderAIComponent::WanderAIComponent(int animIdle, int animMove, float moveSpeed, int thinkRandRange)
	: state_(STATE::NONE)
	, step_(0.0f)
	, moveDir_(AsoUtility::VECTOR_ZERO)
	, animIdle_(animIdle)
	, animMove_(animMove)
	, moveSpeed_(moveSpeed)
	, thinkRandRange_(thinkRandRange)
{
}

void WanderAIComponent::Update(void)
{
	if (!owner_) return;

	if (state_ == STATE::NONE)
	{
		ChangeState(STATE::MOVE);
	}

	switch (state_)
	{
	case STATE::NONE:
		break;
	case STATE::THINK:
		UpdateThink();
		break;
	case STATE::IDLE:
		UpdateIdle();
		break;
	case STATE::MOVE:
		UpdateMove();
		break;
	case STATE::ACTION:
		UpdateAction();
		break;
	case STATE::END:
		break;
	default:
		break;
	}
}

void WanderAIComponent::ChangeState(STATE state)
{
	state_ = state;
	step_ = 0.0f;

	auto physics = owner_->GetComponent<PhysicsComponent>();
	auto anim = owner_->GetComponent<AnimationComponent>();

	if (physics)
	{
		physics->movePow_ = AsoUtility::VECTOR_ZERO;
	}

	switch (state_)
	{
	case STATE::NONE:
		break;
	case STATE::THINK:
	{
		int tmpState = GetRand(thinkRandRange_) + 2;
		ChangeState((STATE)tmpState);
	}
	break;
	case STATE::IDLE:
		if (anim) anim->Play(animIdle_);
		step_ = (float)(2000 + GetRand(3000)) / 1000.0f;
		break;
	case STATE::MOVE:
	{
		if (anim) anim->Play(animMove_);
		step_ = (float)(3000 + GetRand(3000)) / 1000.0f;

		moveDir_ = VECTOR();
		moveDir_.x = ((float)(GetRand(2000)) / 1000.0f) - 1.0f;
		moveDir_.y = 0.0f;
		moveDir_.z = ((float)(GetRand(2000)) / 1000.0f) - 1.0f;
		moveDir_ = VNorm(moveDir_);
	}
	break;
	case STATE::ACTION:
		break;
	case STATE::END:
		break;
	default:
		break;
	}
}

void WanderAIComponent::UpdateThink(void)
{
}

void WanderAIComponent::UpdateIdle(void)
{
	step_ -= SceneManager::GetInstance().GetDeltaTime();
	if (step_ < 0.0f)
	{
		ChangeState(STATE::THINK);
	}
}

void WanderAIComponent::UpdateMove(void)
{
	auto physics = owner_->GetComponent<PhysicsComponent>();
	auto colliderComp = owner_->GetComponent<CapsuleColliderComponent>();
	if (!physics) return;

	float deltaTime = SceneManager::GetInstance().GetDeltaTime();
	float dtScale = deltaTime * 60.0f;

	physics->movePow_ = VScale(moveDir_, moveSpeed_ * dtScale);

	// --- 崖・壁を予測するAI思考 ---
	auto testPos = VAdd(owner_->transform_.pos, VScale(moveDir_, moveSpeed_ * 5.0f));
	VECTOR dirGravity = AsoUtility::DIR_D;
	VECTOR dirUpGravity = AsoUtility::DIR_U;
	float gravityPow = Planet::DEFAULT_GRAVITY_POW;
	float checkPow = 10.0f;

	auto gravHitPosUp = VAdd(testPos, VScale(dirUpGravity, gravityPow));
	gravHitPosUp = VAdd(gravHitPosUp, VScale(dirUpGravity, checkPow * 2.0f));
	auto gravHitPosDown = VAdd(testPos, VScale(dirGravity, checkPow * 10.0f));

	if (colliderComp)
	{
		for (const auto& c : colliderComp->GetColliders())
		{
			if (c.expired()) continue;
			auto hit = MV1CollCheck_Line(c.lock()->modelId_, -1, gravHitPosUp, gravHitPosDown);

			// もし目の前に地面が無い（崖）なら、Uターンする
			if (hit.HitFlag == 0)
			{
				moveDir_ = VScale(moveDir_, -1.0f);
				physics->movePow_ = VScale(moveDir_, 4.0f * dtScale);
			}
		}
	}

	// 進行方向へ滑らかに回転（FPS非依存）
	float rotStep = 0.1f * dtScale;
	if (rotStep > 1.0f) rotStep = 1.0f;
	owner_->transform_.quaRot = Quaternion::Slerp(
		owner_->transform_.quaRot,
		Quaternion::LookRotation(moveDir_),
		rotStep);

	step_ -= deltaTime;
	if (step_ < 0.0f)
	{
		ChangeState(STATE::THINK);
	}
}

void WanderAIComponent::UpdateAction(void)
{
	step_ -= SceneManager::GetInstance().GetDeltaTime();
	if (step_ < 0.0f)
	{
		ChangeState(STATE::THINK);
	}
}
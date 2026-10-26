// Src/Object/Components/PhysicsComponent.cpp
#include "PhysicsComponent.h"
#include "CapsuleColliderComponent.h"
#include "../ActorBase.h"
#include "../Common/Capsule.h"
#include "../Common/Collider.h"
#include "../Planet.h"
#include "../../Manager/SceneManager.h"
#include "../../Utility/AsoUtility.h"

PhysicsComponent::PhysicsComponent(void)
	: speed_(0.0f)
	, moveDir_(AsoUtility::VECTOR_ZERO)
	, movePow_(AsoUtility::VECTOR_ZERO)
	, movedPos_(AsoUtility::VECTOR_ZERO)
	, jumpPow_(AsoUtility::VECTOR_ZERO)
	, isJump_(false)
	, gravHitPosDown_(AsoUtility::VECTOR_ZERO)
	, gravHitPosUp_(AsoUtility::VECTOR_ZERO)
	, isGravityEnabled_(true)
	, onLanding_(nullptr)
{
}

void PhysicsComponent::Update(void)
{
	if (!owner_) return;

	if (isGravityEnabled_)
	{
		CalcGravityPow();
		Collision();
	}

	owner_->transform_.Update();
}

void PhysicsComponent::CalcGravityPow(void)
{
	VECTOR dirGravity = AsoUtility::DIR_D;
	float gravityPow = Planet::DEFAULT_GRAVITY_POW;

	float deltaTime = SceneManager::GetInstance().GetDeltaTime();
	if (deltaTime > 0.025f) {
		deltaTime = 0.025f;
	}
	float dtScale = deltaTime * 60.0f;
	VECTOR gravity = VScale(dirGravity, gravityPow * dtScale);

	// ジャンプ力（落下速度）に重力を足して加速させる
	jumpPow_ = VAdd(jumpPow_, gravity);

	float dot = VDot(dirGravity, jumpPow_);
	// 速度のリセットをやめ、「一定速度以上は速くならない（終端速度）」ようにする
	if (dot > 60.0f) // 30.0fは最大落下速度です。ゲームに合わせて調整してください
	{
		jumpPow_ = VScale(dirGravity, 60.0f);
	}
}

void PhysicsComponent::Collision(void)
{
	if (!owner_) return;
	movedPos_ = VAdd(owner_->transform_.pos, movePow_);
	CollisionCapsule();
	CollisionGravity();
	owner_->transform_.pos = movedPos_;
}

void PhysicsComponent::CollisionCapsule(void)
{
	if (!owner_) return;
	auto colliderComp = owner_->GetComponent<CapsuleColliderComponent>();
	if (!colliderComp || !colliderComp->GetCapsulePtr()) return;

	Transform trans = Transform(owner_->transform_);
	trans.pos = movedPos_;
	trans.Update();
	Capsule cap = Capsule(colliderComp->GetCapsule(), trans);

	for (const auto& c : colliderComp->GetColliders())
	{
		if (c.expired()) continue;
		auto hits = MV1CollCheck_Capsule(c.lock()->modelId_, -1, cap.GetPosTop(), cap.GetPosDown(), cap.GetRadius());
		for (int i = 0; i < hits.HitNum; i++)
		{
			auto hit = hits.Dim[i];
			for (int tryCnt = 0; tryCnt < 10; tryCnt++)
			{
				int pHit = HitCheck_Capsule_Triangle(cap.GetPosTop(), cap.GetPosDown(), cap.GetRadius(), hit.Position[0], hit.Position[1], hit.Position[2]);
				if (pHit)
				{
					movedPos_ = VAdd(movedPos_, VScale(hit.Normal, 1.0f));
					trans.pos = movedPos_;
					trans.Update();
					continue;
				}
				break;
			}
		}
		MV1CollResultPolyDimTerminate(hits);
	}
}

void PhysicsComponent::CollisionGravity(void)
{
	if (!owner_) return;
	auto colliderComp = owner_->GetComponent<CapsuleColliderComponent>();
	if (!colliderComp) return;

	// ★ 修正：ここでも同じように上限を設ける
	float deltaTime = SceneManager::GetInstance().GetDeltaTime();
	if (deltaTime > 0.025f) {
		deltaTime = 0.025f;
	}
	float dtScale = deltaTime * 60.0f;
	movedPos_ = VAdd(movedPos_, VScale(jumpPow_, dtScale));

	VECTOR dirGravity = AsoUtility::DIR_D;
	VECTOR dirUpGravity = AsoUtility::DIR_U;
	float gravityPow = Planet::DEFAULT_GRAVITY_POW;
	float checkPow = 10.0f;

	gravHitPosUp_ = VAdd(movedPos_, VScale(dirUpGravity, gravityPow));
	gravHitPosUp_ = VAdd(gravHitPosUp_, VScale(dirUpGravity, checkPow * 2.0f));
	gravHitPosDown_ = VAdd(movedPos_, VScale(dirGravity, checkPow));

	for (const auto& c : colliderComp->GetColliders())
	{
		if (c.expired()) continue;
		auto hit = MV1CollCheck_Line(c.lock()->modelId_, -1, gravHitPosUp_, gravHitPosDown_);
		if (hit.HitFlag > 0 && VDot(dirGravity, jumpPow_) > 0.0f)
		{
			movedPos_ = VAdd(hit.HitPosition, VScale(dirUpGravity, 2.0f));
			jumpPow_ = AsoUtility::VECTOR_ZERO;

			if (onLanding_)
			{
				onLanding_();
			}
		}
	}
}
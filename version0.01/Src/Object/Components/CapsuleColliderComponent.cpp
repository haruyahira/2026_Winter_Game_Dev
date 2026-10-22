#include "CapsuleColliderComponent.h"
#include "../ActorBase.h"
#include "../Common/Capsule.h"
#include "../Common/Collider.h"

CapsuleColliderComponent::CapsuleColliderComponent(void)
	: capsule_(nullptr)
	, isDrawDebug_(false)
	, debugColor_(GetColor(0, 0, 255))
{
}

CapsuleColliderComponent::~CapsuleColliderComponent(void)
{
}

void CapsuleColliderComponent::Setup(VECTOR top, VECTOR bottom, float radius)
{
	if (!owner_) return;
	capsule_ = std::make_unique<Capsule>(owner_->transform_);
	capsule_->SetLocalPosTop(top);
	capsule_->SetLocalPosDown(bottom);
	capsule_->SetRadius(radius);
}

void CapsuleColliderComponent::Draw(void)
{
	if (!isDrawDebug_ || !capsule_ || !owner_) return;

	Capsule cap = Capsule(*capsule_, owner_->transform_);

	SetUseZBuffer3D(FALSE);
	SetWriteZBuffer3D(FALSE);

	DrawCapsule3D(
		cap.GetPosTop(),
		cap.GetPosDown(),
		cap.GetRadius(),
		16,
		debugColor_,
		GetColor(0, 0, 0),
		FALSE
	);

	SetUseZBuffer3D(TRUE);
	SetWriteZBuffer3D(TRUE);
}

void CapsuleColliderComponent::AddCollider(std::weak_ptr<Collider> collider)
{
	colliders_.push_back(collider);
}

void CapsuleColliderComponent::ClearCollider(void)
{
	colliders_.clear();
}

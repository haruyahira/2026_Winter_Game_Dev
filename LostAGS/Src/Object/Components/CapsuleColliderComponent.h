#pragma once
#include <memory>
#include <vector>
#include <DxLib.h>
#include "../Component.h"

class Capsule;
class Collider;

class CapsuleColliderComponent : public Component
{
public:
	CapsuleColliderComponent(void);
	~CapsuleColliderComponent(void) override;

	void Setup(VECTOR top, VECTOR bottom, float radius);
	void Draw(void) override;

	void AddCollider(std::weak_ptr<Collider> collider);
	void ClearCollider(void);

	const std::vector<std::weak_ptr<Collider>>& GetColliders(void) const { return colliders_; }
	const Capsule& GetCapsule(void) const { return *capsule_; }
	const Capsule* GetCapsulePtr(void) const { return capsule_.get(); }

	void SetDrawDebug(bool isDraw, unsigned int color = 0x0000ff)
	{
		isDrawDebug_ = isDraw;
		debugColor_ = color;
	}

private:
	std::unique_ptr<Capsule> capsule_;
	std::vector<std::weak_ptr<Collider>> colliders_;
	bool isDrawDebug_;
	unsigned int debugColor_;
};

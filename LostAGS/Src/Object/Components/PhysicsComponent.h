#pragma once
#include <functional>
#include <DxLib.h>
#include "../Component.h"

class PhysicsComponent : public Component
{
public:
	PhysicsComponent(void);
	~PhysicsComponent(void) override = default;

	void Update(void) override;

	void CalcGravityPow(void);
	void Collision(void);
	void CollisionCapsule(void);
	void CollisionGravity(void);

	void SetOnLanding(std::function<void(void)> callback) { onLanding_ = callback; }
	void SetGravityEnabled(bool enabled) { isGravityEnabled_ = enabled; }
	bool IsGravityEnabled(void) const { return isGravityEnabled_; }

	// ï®óùââéZópïœêî
	float speed_;
	VECTOR moveDir_;
	VECTOR movePow_;
	VECTOR movedPos_;
	VECTOR jumpPow_;
	bool isJump_;

	VECTOR gravHitPosDown_;
	VECTOR gravHitPosUp_;

private:
	bool isGravityEnabled_;
	std::function<void(void)> onLanding_;
};

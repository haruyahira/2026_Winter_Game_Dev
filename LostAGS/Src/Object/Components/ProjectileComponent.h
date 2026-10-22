#pragma once
#include <functional>
#include <DxLib.h>
#include "../Component.h"

class ProjectileComponent : public Component
{
public:
	ProjectileComponent(float speed = 10.0f, float lifeTime = 5.0f);
	~ProjectileComponent(void) override = default;

	void Fire(VECTOR dir);
	void Update(void) override;

	void SetOnExpire(std::function<void(void)> callback) { onExpire_ = callback; }

private:
	VECTOR moveDir_;
	float speed_;
	float lifeTimer_;
	std::function<void(void)> onExpire_;
};

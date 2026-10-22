#pragma once
#include <memory>
#include "MagicBase.h"

class ProjectileComponent;

class FireBall : public MagicBase
{
public:
	static constexpr int DEFAULT_DAMAGE = 20;
	static constexpr float DEFAULT_SPEED = 10.0f;
	static constexpr float DEFAULT_LIFE_TIME = 5.0f;
	static constexpr float DEFAULT_COLLISION_RADIUS = 30.0f;
	static constexpr float EXPLODE_DURATION = 0.2f; // はじける演出の時間

	FireBall(void);
	virtual ~FireBall(void);

	void Init(void) override;
	void Update(void) override;
	void Draw(void) override;
	void Fire(const Transform& spawnTransform, VECTOR dir) override;
	void OnHit(void) override;
	bool CanHit(void) const override;
	bool isDestroy(void) override;

private:
	std::shared_ptr<ProjectileComponent> projectileComp_;
	bool isExploding_;   // はじけている最中か
	float explodeTimer_; // はじける演出タイマー
};
#pragma once
#include <DxLib.h>
#include "../Component.h"

class StatusComponent : public Component
{
public:
	StatusComponent(int maxHp, float barOffsetY = 140.0f);
	~StatusComponent(void) override = default;

	void Update(void) override;
	void Draw(void) override;

	// ダメージを受ける
	void TakeDamage(int damage);

	int GetHp(void) const { return hp_; }
	int GetMaxHp(void) const { return maxHp_; }
	bool IsAlive(void) const { return isAlive_; }

private:
	int hp_;
	int maxHp_;
	bool isAlive_;
	float barOffsetY_;  // 頭上HPバーの高さオフセット
	float damageFlashTimer_;  // 被弾時の赤点滅タイマー
};
#include "StatusComponent.h"
#include "../ActorBase.h"
#include "../../Manager/SceneManager.h"

StatusComponent::StatusComponent(int maxHp, float barOffsetY)
	: hp_(maxHp)
	, maxHp_(maxHp)
	, isAlive_(true)
	, barOffsetY_(barOffsetY)
	, damageFlashTimer_(0.0f)
{
}

void StatusComponent::Update(void)
{
	if (damageFlashTimer_ > 0.0f)
	{
		damageFlashTimer_ -= SceneManager::GetInstance().GetDeltaTime();
	}
}

void StatusComponent::Draw(void)
{
	if (!owner_ || !isAlive_) return;

	// 頭上のワールド座標をスクリーン座標に変換してHPバーを表示
	VECTOR worldPos = VAdd(owner_->transform_.pos, VGet(0.0f, barOffsetY_, 0.0f));
	VECTOR screenPos = ConvWorldPosToScreenPos(worldPos);

	// カメラの描画範囲内の場合のみ描画
	if (screenPos.z > 0.0f && screenPos.z < 1.0f)
	{
		const int barWidth = 60;
		const int barHeight = 8;
		int x = static_cast<int>(screenPos.x) - (barWidth / 2);
		int y = static_cast<int>(screenPos.y);

		float hpRate = static_cast<float>(hp_) / static_cast<float>(maxHp_);
		int currentWidth = static_cast<int>(barWidth * hpRate);

		// 背景枠（黒・赤）と現在HP（緑、被弾時は黄色）
		DrawBox(x - 1, y - 1, x + barWidth + 1, y + barHeight + 1, GetColor(0, 0, 0), TRUE);
		DrawBox(x, y, x + barWidth, y + barHeight, GetColor(180, 0, 0), TRUE);

		unsigned int barColor = (damageFlashTimer_ > 0.0f) ? GetColor(255, 255, 0) : GetColor(0, 220, 0);
		DrawBox(x, y, x + currentWidth, y + barHeight, barColor, TRUE);
	}
}

void StatusComponent::TakeDamage(int damage)
{
	if (!isAlive_) return;

	hp_ -= damage;
	damageFlashTimer_ = 0.2f;

	if (hp_ <= 0)
	{
		hp_ = 0;
		isAlive_ = false;
	}
}
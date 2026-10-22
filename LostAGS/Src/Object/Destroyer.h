#pragma once
#include <DxLib.h>
#include "Charactor/Enemy/EnemyBase.h"

class Destroyer : public EnemyBase
{
public:

	// 状態
	enum class STATE
	{
		NONE,
		THINK,
		IDLE,
		MOVE,
		ACTION,
		END
	};

	// アニメーション種別（元の定数 3, 10 をそのまま維持）
	enum class ANIM_TYPE
	{
		IDLE = 3,
		RUN = 10,
	};

	// 初期座標・初期角度・カプセル設定（元の定数をそのまま維持）
	static constexpr VECTOR DEFAULT_POS = { 150.0f, -40.0f, 200.0f };
	static constexpr float DEFAULT_ROT_Y_DEG = 180.0f;
	static constexpr float INIT_SCALE = 1.0f;

	static constexpr VECTOR CAPSULE_TOP = { 0.0f, 110.0f, 0.0f };
	static constexpr VECTOR CAPSULE_BOTTOM = { 0.0f, 30.0f, 0.0f };
	static constexpr float CAPSULE_RADIUS = 20.0f;
	static constexpr float SHADOW_SIZE = 50.0f;

	Destroyer(void);
	~Destroyer(void) override;

protected:

	void InitDetail(void) override;

private:

	void InitAnimation(void);
};

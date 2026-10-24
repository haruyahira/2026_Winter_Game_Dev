// BossEnemy.h
#pragma once
#include "EnemyBase.h"

class BossEnemy : public EnemyBase
{
public:

	// モデルの基本設定（変更禁止の定数そのまま維持）
	static constexpr VECTOR INIT_POSITION = { 150.0f, 30.0f, 200.0f };  // ボス初期座標
	static constexpr VECTOR INIT_AXIS = { 0.0f, 180.0f, 0.0f };  // ボス初期軸
	static constexpr float INIT_SCALE = 0.3f;  // ボス初期スケール

	// 当たり判定（カプセル）のサイズ設定
	static constexpr VECTOR CAPSULE_TOP = { 0.0f, 150.0f, 0.0f };  // カプセル上端座標
	static constexpr VECTOR CAPSULE_BOTTOM = { 0.0f, 50.0f, 0.0f };  // カプセル下端座標
	static constexpr float CAPSULE_RADIUS = 60.0f;  // カプセルの半径

	// ボスの最大HP
	static constexpr int MAX_HP = 100;

	// ボスのAI状態
	enum class STATE
	{
		NONE,
		THINK,
		IDLE,
		MOVE,
		ACTION,
		END
	};

	// アニメーション種別（FBX内のアニメーション番号 13, 14 をそのまま維持）
	enum class ANIM_TYPE
	{
		IDLE = 13,
		WALK = 14,
	};

	BossEnemy(void);
	virtual ~BossEnemy(void);

protected:

	void InitDetail(void) override;

private:

	void InitAnimation(void);
};
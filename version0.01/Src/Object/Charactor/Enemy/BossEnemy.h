#pragma once
#include "EnemyBase.h"

class BossEnemy : public EnemyBase
{

public:

	// モデルの基本設定
	static constexpr VECTOR INIT_POSITION = { 150.0f, -40.0f, 200.0f };  // ボス初期座標
	static constexpr VECTOR INIT_AXIS = { 0.0f, 180.0f, 0.0f };  // ボス初期軸
	static constexpr float INIT_SCALE = 0.3f;  // ボス初期スケール

	// 当たり判定（カプセル）のサイズ設定
	static constexpr VECTOR CAPSULE_TOP = { 0.0f, 110.0f, 0.0f };  // カプセル上端座標
	static constexpr VECTOR CAPSULE_BOTTOM = { 0.0f, 30.0f, 0.0f };  // カプセル下端座標
	static constexpr float CAPSULE_RADIUS = 60.0f;  // カプセルの半径

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

	// アニメーション種別
	enum class ANIM_TYPE
	{
		IDLE = 13,
		WALK = 14,
	};

	BossEnemy(void);
	~BossEnemy(void);

	void InitDetail(void) override;
	void InitAnimation(void);

	// EnemyBaseから毎フレーム呼ばれる
	void ProcessAI(void) override;
};
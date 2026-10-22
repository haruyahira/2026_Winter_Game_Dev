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

	// コンストラクタ
	BossEnemy(void);

	// デストラクタ
	virtual ~BossEnemy(void);

protected:

	// CharactorBase から委譲された初期化（モデル読み込み等）
	void InitDetail(void) override;

	// EnemyBase から委譲されたAI思考処理（毎フレーム呼ばれる）
	void ProcessAI(void) override;

private:

	// ボス専用の変数
	STATE state_;  // 現在のAI状態
	float step_;  // 汎用タイマー

	// 初期化
	void InitAnimation(void);

	// 状態遷移
	void ChangeState(STATE state);

	// 各ステートごとの更新関数
	void UpdateThink(void);
	void UpdateIdle(void);
	void UpdateMove(void);
	void UpdateAction(void);
};
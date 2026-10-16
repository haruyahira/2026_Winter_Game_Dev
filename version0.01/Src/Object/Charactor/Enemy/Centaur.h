#pragma once
#include "EnemyBase.h"

class Centaur : public EnemyBase
{
public:
	// ケンタウロス独自のAIステート
	enum class CENTAUR_STATE
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
	Centaur(void);

	// デストラクタ
	virtual ~Centaur(void);

protected:
	// CharactorBase から委譲された初期化（モデル読み込み等）
	void InitDetail(void) override;

	// EnemyBase から委譲されたAI思考処理（毎フレーム呼ばれる）
	void ProcessAI(void) override;

private:
	// ケンタウロス専用の変数
	CENTAUR_STATE cState_; // 現在のAIステート
	float step_;           // 汎用タイマー

	// 状態遷移
	void ChangeState(CENTAUR_STATE state);

	// 各ステートごとの更新関数
	void UpdateThink(void);
	void UpdateIdle(void);
	void UpdateMove(void);
	void UpdateAction(void);
};
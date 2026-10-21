#pragma once
#include <memory>
#include "../CharactorBase.h"

// プレイヤーの位置を知るために必要
class Player;

class EnemyBase : public CharactorBase

{
public:

	// 敵の共通状態
	enum class STATE
	{
		NONE,
		THINK,
		IDLE,
		MOVE,
		ACTION,
		END
	};

	EnemyBase(void);
	virtual ~EnemyBase(void);

	// EnemyBase共通の更新・描画
	virtual void Update(void) override;
	virtual void Draw(void) override;

	// 狙う相手（プレイヤー）をセットする
	void SetTarget(std::weak_ptr<Player> target);

protected:

	// 敵専用の変数
	STATE state_;
	std::weak_ptr<Player> target_;	// プレイヤーの場所を知るための参照

	// 敵共通の思考処理（子クラスで上書きできるようにしておく）
	virtual void ProcessAI(void);

	// CharactorBaseから委譲された着地処理
	virtual void OnLanding(void) override;
};
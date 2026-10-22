#pragma once
#include <memory>
#include "../CharactorBase.h"

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

	// EnemyBase共通の更新・描画（コンポーネントを一括更新・描画）
	virtual void Update(void) override;
	virtual void Draw(void) override;

	// 狙う相手（プレイヤー）をセットする
	void SetTarget(std::weak_ptr<Player> target);
	std::weak_ptr<Player> GetTarget(void) const { return target_; }

protected:

	STATE state_;
	std::weak_ptr<Player> target_;

	virtual void ProcessAI(void);
	virtual void OnLanding(void) override;
};

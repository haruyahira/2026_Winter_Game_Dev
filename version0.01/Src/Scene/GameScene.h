// GameScene.h
#pragma once
#include <memory>
#include <vector>
#include "SceneBase.h"
class ActorBase;
class EnemyBase;
class Stage;
class SkyDome;
class Player;
class Destroyer;
class BossEnemy;

class GameScene : public SceneBase
{

public:

	// コンストラクタ
	GameScene(void);

	// デストラクタ
	~GameScene(void);

	void Init(void) override;
	void Update(void) override;
	void Draw(void) override;

private:

	// 魔法
	std::vector<std::unique_ptr<ActorBase>> dynamicActors_;

	// ステージ
	std::unique_ptr<Stage> stage_;

	// スカイドーム
	std::unique_ptr<SkyDome> skyDome_;

	// プレイヤー
	std::unique_ptr<Player> player_;

	// 破壊者
	std::unique_ptr<Destroyer> destroyer_;

	// ケンタウロス
	std::unique_ptr<BossEnemy> centaur_;

	// クリア遷移用タイマー
	float clearTimer_;
	bool isStageCleared_;

	// 魔法と敵の当たり判定
	void CheckCollision(void);
};
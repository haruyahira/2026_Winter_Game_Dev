// Src/Scene/GameScene.h
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

	GameScene(void);
	~GameScene(void);

	void Init(void) override;
	void Update(void) override;
	void Draw(void) override;

private:

	std::vector<std::unique_ptr<ActorBase>> dynamicActors_;
	std::unique_ptr<Stage> stage_;
	std::unique_ptr<SkyDome> skyDome_;
	std::unique_ptr<Player> player_;
	std::unique_ptr<Destroyer> destroyer_;
	std::unique_ptr<BossEnemy> centaur_;

	// ★ クリア遷移用タイマー
	float clearTimer_;
	bool isStageCleared_;

	void CheckCollision(void);
};
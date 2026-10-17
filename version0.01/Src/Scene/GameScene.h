#pragma once
#include <memory>
#include "SceneBase.h"
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

};

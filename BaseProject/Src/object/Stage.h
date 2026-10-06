#pragma once
#include <memory>
#include <map>
#include "Common/Transform.h"
class ResourceManager;
class Planet;
class Skull;
class Player;
class Destroyer;
class Centaur;

class Stage
{

public:

	// ステージの切り替え間隔
	static constexpr float TIME_STAGE_CHANGE = 1.0f;

	// ステージ名
	enum class NAME
	{
		MAIN_PLANET,
	};

	// コンストラクタ
	Stage(Player& player, Destroyer& destroyer, Centaur& centaur);

	// デストラクタ
	~Stage(void);

	void Init(void);
	void Update(void);
	void Draw(void);

	// ステージ変更
	void ChangeStage(NAME type);

	// 対象ステージを取得
	std::weak_ptr<Planet> GetPlanet(NAME type);

private:

	// シングルトン参照
	ResourceManager& resMng_;

	Player& player_;
	Destroyer& destroyer_;
	Centaur& centaur_;

	// ステージアクティブになっている惑星の情報
	NAME activeName_;
	std::weak_ptr<Planet> activePlanet_;

	// 惑星
	std::map<NAME, std::shared_ptr<Planet>> planets_;

	// 骸骨オブジェクト
	std::vector<std::shared_ptr<Skull>> skulls_;

	// 空のPlanet
	std::shared_ptr<Planet> nullPlanet = nullptr;

	float step_;

	// 最初の惑星
	void MakeMainStage(void);

	// 環境オブジェクト
	void MakeEnvironment(void);

};

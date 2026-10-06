#include <vector>
#include <map>
#include <DxLib.h>
#include "../Utility/AsoUtility.h"
#include "../Manager/SceneManager.h"
#include "../Manager/ResourceManager.h"
#include "Player.h"
#include "Destroyer.h"
#include "Centaur.h"
#include "Planet.h"
#include "Skull.h"
#include "Common/Collider.h"
#include "Common/Transform.h"
#include "Stage.h"

Stage::Stage(Player& player, Destroyer& destroyer, Centaur& centaur)
	:
	resMng_(ResourceManager::GetInstance()),
	player_(player), destroyer_(destroyer), centaur_(centaur)
{
	activeName_ = NAME::MAIN_PLANET;
	step_ = 0.0f;
}

Stage::~Stage(void)
{
	
	// 惑星
	planets_.clear();

	// 骸骨オブジェクト
	skulls_.clear();

}

void Stage::Init(void)
{

	MakeMainStage();
	
	MakeEnvironment();
	
	step_ = -1.0f;

}

void Stage::Update(void)
{

	// 惑星
	for (const auto& s : planets_)
	{
		s.second->Update();
	}

	// 骸骨オブジェクト
	for (const auto& s : skulls_)
	{
		s->Update();
	}

}

void Stage::Draw(void)
{

	// 惑星
	for (const auto& s : planets_)
	{
		s.second->Draw();
	}

	// 骸骨オブジェクト
	for (const auto& s : skulls_)
	{
		s->Draw();
	}

}

void Stage::ChangeStage(NAME type)
{

	activeName_ = type;

	// 対象のステージを取得する
	activePlanet_ = GetPlanet(activeName_);

	// ステージの当たり判定をプレイヤーに設定
	player_.ClearCollider();
	player_.AddCollider(activePlanet_.lock()->GetTransform().collider);
	destroyer_.ClearCollider();
	destroyer_.AddCollider(activePlanet_.lock()->GetTransform().collider);
	centaur_.ClearCollider();
	centaur_.AddCollider(activePlanet_.lock()->GetTransform().collider);

	step_ = TIME_STAGE_CHANGE;

}

std::weak_ptr<Planet> Stage::GetPlanet(NAME type)
{
	if (planets_.count(type) == 0)
	{
		return nullPlanet;
	}

	return planets_[type];
}

void Stage::MakeMainStage(void)
{

	// 最初の惑星
	//------------------------------------------------------------------------------
	Transform planetTrans;
	planetTrans.SetModel(
		resMng_.LoadModelDuplicate(ResourceManager::SRC::MAIN_PLANET));
	planetTrans.scl = AsoUtility::VECTOR_ONE;
	planetTrans.quaRot = Quaternion();
	planetTrans.pos = { 0.0f, -760.0f, 0.0f };

	// 当たり判定(コライダ)作成
	planetTrans.MakeCollider(Collider::TYPE::STAGE);

	planetTrans.Update();

	NAME name = NAME::MAIN_PLANET;
	std::shared_ptr<Planet> planet =
		std::make_shared<Planet>(
			name, Planet::TYPE::GROUND, planetTrans);
	planet->Init();
	planets_.emplace(name, std::move(planet));
	//------------------------------------------------------------------------------

}

void Stage::MakeEnvironment(void)
{

	auto skull = std::make_shared<Skull>();
	skull->Init();
	skull->SetPos({ -300.0f, -30.0f, 500.0f });
	skulls_.emplace_back(std::move(skull));

	skull = std::make_shared<Skull>();
	skull->Init();
	skull->SetPos({ 250.0f, -60.0f, 900.0f });
	skulls_.emplace_back(std::move(skull));

	skull = std::make_shared<Skull>();
	skull->Init();
	skull->SetPos({ -20.0f, -45.0f, 1600.0f });
	skulls_.emplace_back(std::move(skull));

}

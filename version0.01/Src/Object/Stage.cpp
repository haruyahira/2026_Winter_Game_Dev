#include <vector>
#include <map>
#include <DxLib.h>
#include <EffekseerForDXLib.h>
#include "../Utility/AsoUtility.h"
#include "../Manager/SceneManager.h"
#include "../Manager/ResourceManager.h"
#include "../Manager/LightManager.h"
#include "Charactor/Player/Player.h"
#include "Charactor/Enemy/BossEnemy.h"
#include "Common/Collider.h"
#include "Common/Transform.h"
#include "Destroyer.h"
#include "Planet.h"
#include "Skull.h"
#include "Stage.h"

Stage::Stage(Player& player, Destroyer& destroyer, BossEnemy& centaur)
	:
	resMng_(ResourceManager::GetInstance()),
	player_(player), destroyer_(destroyer), bossEnemy_(centaur)
{
	activeName_ = NAME::MAIN_PLANET;
	step_ = 0.0f;

	lightManager_ = new LightManager();
}

Stage::~Stage(void)
{
	delete lightManager_;

	// 惑星
	planets_.clear();

	// 修正：保存した全てのエフェクトハンドルを停止する
	for (int handle : flamePlayingHandles_)
	{
		StopEffekseer3DEffect(handle);
	}
	flamePlayingHandles_.clear();
	DeleteEffekseerEffect(flameEffectRes_);

	// 骸骨オブジェクト
	//skulls_.clear();
}

void Stage::Init(void)
{
	MakeMainStage();
	step_ = -1.0f;

	// 環境光と標準ライトをゼロに
	SetGlobalAmbientLight(GetColorF(0.0f, 0.0f, 0.0f, 1.0f));
	SetLightDifColor(GetColorF(0.0f, 0.0f, 0.0f, 1.0f));
	SetLightSpcColor(GetColorF(0.0f, 0.0f, 0.0f, 1.0f));
	SetLightAmbColor(GetColorF(0.0f, 0.0f, 0.0f, 1.0f));

	flameEffectRes_ = LoadEffekseerEffect("Data/Effect/BlueFlame/BlueFlame.efkefc", 168.0f);

	// ライトマネージャーの初期化
	lightManager_->Clear();

	if (flameEffectRes_ != -1 && planets_.find(NAME::MAIN_PLANET) != planets_.end())
	{
		int modelHandle = planets_[NAME::MAIN_PLANET]->GetTransform().modelId;

		for (int i = 0; i <= 31; ++i)
		{
			char frameName[64];
			if (i == 0) sprintf_s(frameName, "Wall_Torch");
			else        sprintf_s(frameName, "Wall_Torch.%03d", i);

			int frameIndex = MV1SearchFrame(modelHandle, frameName);
			if (frameIndex != -1)
			{
				VECTOR torchPos = MV1GetFramePosition(modelHandle, frameIndex);
				if (torchPos.x > 0.0f) torchPos.x -= 200.0f;
				else                   torchPos.x += 200.0f;
				torchPos.y += 140.0f;

				int handle = PlayEffekseer3DEffect(flameEffectRes_);
				SetPosPlayingEffekseer3DEffect(handle, torchPos.x, torchPos.y, torchPos.z);
				flamePlayingHandles_.push_back(handle);

				// 修正後（距離と光の強さを上げる）
				lightManager_->AddPointLight(
					torchPos,
					4100.0f,                            // ★床の中央まで届くように 6000 に広げる
					GetColorF(0.0f, 0.8f, 2.0f, 1.0f)   // ★ライトが重なっても白飛びしない強さに調整
				);
			}
		}

		// GPUに転送（動くライトがある場合は Update() 内で毎フレーム呼んでください）
		lightManager_->UpdateBuffer();
	}
}

void Stage::Update(void)
{
	UpdateEffekseer3D();

	// 惑星
	for (const auto& s : planets_)
	{
		s.second->Update();
	}
}

void Stage::Draw(void)
{
	lightManager_->SetReservePS(5);

	// 惑星
	for (const auto& s : planets_)
	{  
		s.second->Draw();
	}

	DrawEffekseer3D();
}

void Stage::ChangeStage(NAME type)
{
	activeName_ = type;

	// 対象のステージを取得する
	activePlanet_ = GetPlanet(activeName_);

	// ステージの当たり判定を設定
	player_.ClearCollider();
	player_.AddCollider(activePlanet_.lock()->GetTransform().collider);
	bossEnemy_.ClearCollider();
	bossEnemy_.AddCollider(activePlanet_.lock()->GetTransform().collider);

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
	/*planetTrans.scl = { 5.0f, 5.0f, 5.0f };*/
	planetTrans.scl = { 1.0f, 1.0f, 1.0f };
	planetTrans.quaRot = Quaternion();
	planetTrans.pos = { 0.0f, 0.0f, 0.0f };

	planetTrans.Update();

	// 当たり判定(コライダ)作成
	planetTrans.MakeCollider(Collider::TYPE::STAGE);

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

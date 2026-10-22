#include <DxLib.h>
#include "../Utility/AsoUtility.h"
#include "../Manager/SceneManager.h"
#include "../Manager/Camera.h"
#include "../Manager/InputManager.h"
#include "../Object/Common/Capsule.h"
#include "../Object/Common/Collider.h"
#include "../Object/Charactor/Player/Player.h"
#include "../Object/Charactor/Enemy/BossEnemy.h"
#include "../Object/SkyDome.h"
#include "../Object/Stage.h"
#include "../Object/Destroyer.h"
#include "GameScene.h"

GameScene::GameScene(void)
{
	player_ = nullptr;
	destroyer_ = nullptr;
	skyDome_ = nullptr;
	stage_ = nullptr;
}

GameScene::~GameScene(void)
{
}

void GameScene::Init(void)
{

	// プレイヤー
	player_ = std::make_unique<Player>();
	player_->Init();
	player_->SetSpawnFunc([this](std::unique_ptr<ActorBase> actor) {
		this->dynamicActors_.push_back(std::move(actor));
		});
	
	// 破壊者
	destroyer_ = std::make_unique<Destroyer>();
	destroyer_->Init();

	// 破壊者
	centaur_ = std::make_unique<BossEnemy>();
	centaur_->Init();

	// ステージ
	stage_ = std::make_unique<Stage>(*player_, *destroyer_, *centaur_);
	stage_->Init();

	// ステージの初期設定
	stage_->ChangeStage(Stage::NAME::MAIN_PLANET);

	// スカイドーム
	skyDome_ = std::make_unique<SkyDome>(player_->GetTransform());
	skyDome_->Init();

	mainCamera.SetFollow(&player_->GetTransform());
	mainCamera.ChangeMode(Camera::MODE::FOLLOW);

}

void GameScene::Update(void)
{

	// シーン遷移
	InputManager& ins = InputManager::GetInstance();
	if (ins.IsTrgDown(KEY_INPUT_1))
	{
		SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::TITLE);
	}

	skyDome_->Update();

	stage_->Update();

	player_->Update();

	destroyer_->Update();

	centaur_->Update();

	for (auto it = dynamicActors_.begin(); it != dynamicActors_.end(); )
	{
		(*it)->Update();

		if ((*it)->isDestroy())
		{
			it = dynamicActors_.erase(it); // 消滅！
		}
		else
		{
			++it;
		}
	}
}

void GameScene::Draw(void)
{

	// 背景
	skyDome_->Draw();
	stage_->Draw();
	
	player_->Draw();
	destroyer_->Draw();
	centaur_->Draw();

	for (auto& actor : dynamicActors_)
	{
		actor->Draw();
	}

	// ヘルプ
	DrawFormatString(840, 20, 0x000000, "移動　　：WASD");
	DrawFormatString(840, 40, 0x000000, "カメラ　：矢印キー");
	DrawFormatString(840, 60, 0x000000, "ダッシュ：右Shift");
	DrawFormatString(840, 80, 0x000000, "ジャンプ：SPACE");

	// 今ゲーム内に存在している魔法の数を画面左上に表示する
	DrawFormatString(20, 20, 0xffffff, "魔法の数：%d", dynamicActors_.size());
	if (!dynamicActors_.empty())
	{
		auto pos = dynamicActors_[0]->GetTransform().pos;
		DrawFormatString(20, 40, 0xffffff, "魔法の座標: X:%.1f Y:%.1f Z:%.1f", pos.x, pos.y, pos.z);
	}
	auto playerPos = player_->GetTransform().pos;
	DrawFormatString(20, 60, 0xffffff, "座標: X:%.1f Y:%.1f Z:%.1f", playerPos.x, playerPos.y, playerPos.z);
}

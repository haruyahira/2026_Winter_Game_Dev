// Src/Scene/GameScene.cpp
#include <DxLib.h>
#include "../Application.h"
#include "../Utility/AsoUtility.h"
#include "../Manager/SceneManager.h"
#include "../Manager/Camera.h"
#include "../Manager/InputManager.h"
#include "../Object/Common/Capsule.h"
#include "../Object/Common/Collider.h"
#include "../Object/Charactor/Player/Player.h"
#include "../Object/Charactor/Enemy/BossEnemy.h"
#include "../Object/Magic/MagicBase.h"
#include "../Object/Components/CapsuleColliderComponent.h"
#include "../Object/Components/StatusComponent.h"
#include "../Object/SkyDome.h"
#include "../Object/Stage.h"
#include "../Object/Destroyer.h"
#include "GameScene.h"

GameScene::GameScene(void)
	: player_(nullptr)
	, destroyer_(nullptr)
	, centaur_(nullptr)
	, skyDome_(nullptr)
	, stage_(nullptr)
	, clearTimer_(0.0f)
	, isStageCleared_(false)
{
}

GameScene::~GameScene(void)
{
}

void GameScene::Init(void)
{
	clearTimer_ = 0.0f;
	isStageCleared_ = false;

	// プレイヤー
	player_ = std::make_unique<Player>();
	player_->Init();
	player_->SetSpawnFunc([this](std::unique_ptr<ActorBase> actor) {
		this->dynamicActors_.push_back(std::move(actor));
		});
	
	// 破壊者
	/*destroyer_ = std::make_unique<Destroyer>();
	destroyer_->Init();*/

	// ケンタウロス
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
	InputManager& ins = InputManager::GetInstance();
	if (ins.IsTrgDown(KEY_INPUT_1))
	{
		SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::TITLE);
		return;
	}

	skyDome_->Update();
	stage_->Update();
	player_->Update();
	//destroyer_->Update();
	centaur_->Update();

	for (auto& actor : dynamicActors_)
	{
		actor->Update();
	}

	// 魔法と敵の当たり判定
	CheckCollision();

	// 消滅した魔法をリストから削除
	for (auto it = dynamicActors_.begin(); it != dynamicActors_.end(); )
	{
		if ((*it)->isDestroy())
		{
			it = dynamicActors_.erase(it);
		}
		else
		{
			++it;
		}
	}

	// ★ すべての敵（ボスと破壊者）を倒したら2秒後にクリア画面へ遷移！
	bool bossDead = (!centaur_ || centaur_->isDestroy());
	//bool zakoDead = (!destroyer_ || destroyer_->isDestroy());

	if (bossDead && /*zakoDead &&*/ !isStageCleared_)
	{
		clearTimer_ += SceneManager::GetInstance().GetDeltaTime();
		if (clearTimer_ >= 2.0f)
		{
			isStageCleared_ = true;
			SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::CLEAR);
		}
	}
}

void GameScene::CheckCollision(void)
{
	EnemyBase* enemies[] = { centaur_.get(), destroyer_.get() };

	for (auto& actor : dynamicActors_)
	{
		auto magic = dynamic_cast<MagicBase*>(actor.get());
		if (!magic || !magic->CanHit()) continue;

		VECTOR magicPos = magic->GetTransform().pos;
		float magicRadius = magic->GetCollisionRadius();

		for (EnemyBase* enemy : enemies)
		{
			if (!enemy || enemy->isDestroy()) continue;

			auto capComp = enemy->GetComponent<CapsuleColliderComponent>();
			auto statusComp = enemy->GetComponent<StatusComponent>();
			if (!capComp || !capComp->GetCapsulePtr() || !statusComp) continue;

			const Capsule& cap = capComp->GetCapsule();

			if (AsoUtility::IsHitSphereCapsule(
				magicPos, magicRadius,
				cap.GetPosTop(), cap.GetPosDown(), cap.GetRadius()))
			{
				statusComp->TakeDamage(magic->GetDamage());
				magic->OnHit();
				break;
			}
		}
	}
}

void GameScene::Draw(void)
{
	skyDome_->Draw();
	stage_->Draw();
	
	player_->Draw();
	//destroyer_->Draw();
	centaur_->Draw();

	for (auto& actor : dynamicActors_)
	{
		actor->Draw();
	}

	int screenW = Application::GetScreenWidth();
	int screenH = Application::GetScreenHeight();

	// 画面中央に照準（クロスヘア）を描画
	int cx = screenW / 2;
	int cy = screenH / 2;
	DrawCircle(cx, cy, 4, GetColor(0, 255, 255), FALSE);
	DrawLine(cx - 10, cy, cx - 4, cy, GetColor(0, 255, 255));
	DrawLine(cx + 4, cy, cx + 10, cy, GetColor(0, 255, 255));
	DrawLine(cx, cy - 10, cx, cy - 4, GetColor(0, 255, 255));
	DrawLine(cx, cy + 4, cx, cy + 10, GetColor(0, 255, 255));

	// 操作ヘルプ
	int helpX = screenW - 260;
	DrawFormatString(helpX, 20, 0x000000, "移動  ：WASD");
	DrawFormatString(helpX, 45, 0x000000, "視点回転：マウス移動");
	DrawFormatString(helpX, 70, 0x000000, "ダッシュ：Shift");
	DrawFormatString(helpX, 95, 0x000000, "ジャンプ：SPACE");
	DrawFormatString(helpX, 120, 0x000000, "魔法詠唱：TAB / 速射：左クリック");

	// 左上ステータス
	DrawFormatString(20, 20, 0xffffff, "魔法の数：%d", (int)dynamicActors_.size());
	auto playerPos = player_->GetTransform().pos;
	DrawFormatString(20, 45, 0xffffff, "座標: X:%.1f Y:%.1f Z:%.1f", playerPos.x, playerPos.y, playerPos.z);

	// 敵撃破後のクリア演出メッセージ
	if (clearTimer_ > 0.0f)
	{
		DrawFormatString(cx - 80, cy - 60, GetColor(255, 255, 0), "ALL ENEMIES DEFEATED!");
	}
}
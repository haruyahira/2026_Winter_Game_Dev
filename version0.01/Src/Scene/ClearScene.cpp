#include <cmath>
#include <algorithm>
#include <cctype>
#include <DxLib.h>
#include "../Application.h"
#include "../Utility/AsoUtility.h"
#include "../Manager/SceneManager.h"
#include "../Manager/ResourceManager.h"
#include "../Manager/InputManager.h"
#include "../Manager/Camera.h"
#include "../Object/SkyDome.h"
#include "../Object/Magic/SystemMagic.h"
#include "../Object/Common/AnimationController.h"
#include "../Object/Charactor/Player/Player.h"
#include "../Object/Charactor/Player/ChantSystem.h"
#include "ClearScene.h"

ClearScene::ClearScene(void)
	: playerAnim_(nullptr)
	, skyDome_(nullptr)
	, chantSystem_(nullptr)
	, animTime_(0.0f)
	, isError_(false)
	, errorTimer_(0.0f)
	, fontClear_(-1)
	, fontSub_(-1)
	, fontGuide_(-1)
	, lastScreenH_(0)
{
}

ClearScene::~ClearScene(void)
{
	if (fontClear_ != -1) DeleteFontToHandle(fontClear_);
	if (fontSub_ != -1) DeleteFontToHandle(fontSub_);
	if (fontGuide_ != -1) DeleteFontToHandle(fontGuide_);
}

void ClearScene::UpdateFonts(int screenH)
{
	if (screenH == lastScreenH_ && fontClear_ != -1) return;

	if (fontClear_ != -1) DeleteFontToHandle(fontClear_);
	if (fontSub_ != -1) DeleteFontToHandle(fontSub_);
	if (fontGuide_ != -1) DeleteFontToHandle(fontGuide_);

	lastScreenH_ = screenH;

	int clearSize = static_cast<int>(screenH * 0.090f);
	int subSize = static_cast<int>(screenH * 0.032f);
	int guideSize = static_cast<int>(screenH * 0.032f);

	if (clearSize < 40) clearSize = 40;
	if (subSize < 16)   subSize = 16;
	if (guideSize < 18) guideSize = 18;

	fontClear_ = CreateFontToHandle(NULL, clearSize, 6, DX_FONTTYPE_ANTIALIASING_EDGE_8X8);
	fontSub_ = CreateFontToHandle(NULL, subSize, 3, DX_FONTTYPE_ANTIALIASING_EDGE_4X4);
	fontGuide_ = CreateFontToHandle(NULL, guideSize, 3, DX_FONTTYPE_ANTIALIASING_EDGE_4X4);
}

void ClearScene::Init(void)
{
	// 定点カメラ（デフォルト位置 {0, 100, -500} から原点方向を見る）
	mainCamera.ChangeMode(Camera::MODE::FIXED_POINT);

	// 1. 地面となる惑星を配置
	planetTransform_.SetModel(resMng_.LoadModelDuplicate(ResourceManager::SRC::MAIN_PLANET));
	planetTransform_.scl = AsoUtility::VECTOR_ONE;
	planetTransform_.pos = { 0.0f, -760.0f, 0.0f };
	planetTransform_.quaRot = Quaternion();
	planetTransform_.Update();

	// 2. カメラの目の前にプレイヤーを配置し、勝利アニメーション（VICTORY）を再生！
	playerTransform_.SetModel(resMng_.LoadModelDuplicate(ResourceManager::SRC::PLAYER));
	playerTransform_.scl = VScale(AsoUtility::VECTOR_ONE, 1.2f);
	playerTransform_.pos = { 0.0f, -15.0f, -280.0f };
	playerTransform_.quaRot = Quaternion();
	playerTransform_.quaRotLocal = Quaternion(); // カメラ正面を向く
	playerTransform_.Update();

	std::string path = Application::PATH_MODEL + "Player/";
	playerAnim_ = std::make_unique<AnimationController>(playerTransform_.modelId);
	playerAnim_->Add((int)Player::ANIM_TYPE::VICTORY, path + "Victory.mv1", 50.0f);
	playerAnim_->Play((int)Player::ANIM_TYPE::VICTORY, true);

	// 3. 背景スカイドーム
	skyDome_ = std::make_unique<SkyDome>(playerTransform_);
	skyDome_->Init();

	// 4. クリア画面用の詠唱システム準備
	chantSystem_ = std::make_unique<ChantSystem>();
	chantSystem_->SetHintText("【Enter/左ｸﾘｯｸ】帰還発動 / 【Tab】閉じる\n帰還の呪文: title / 終了の呪文: exit");
}

void ClearScene::Update(void)
{
	float deltaTime = SceneManager::GetInstance().GetDeltaTime();
	animTime_ += deltaTime;

	if (errorTimer_ > 0.0f)
	{
		errorTimer_ -= deltaTime;
		if (errorTimer_ <= 0.0f) isError_ = false;
	}

	skyDome_->Update();
	if (playerAnim_)
	{
		playerAnim_->Update();
	}

	InputManager& ins = InputManager::GetInstance();

	// 詠唱モード中の処理
	if (chantSystem_->IsChanting())
	{
		if (ins.IsTrgDown(KEY_INPUT_TAB))
		{
			chantSystem_->Cancel();
			return;
		}

		chantSystem_->Update();

		if (chantSystem_->IsFinished())
		{
			// スペース削除・小文字化を1行で取得！
			std::string spell = chantSystem_->GetNormalizedSpell();

			// ★ SystemMagic クラスに判定を丸投げする！（titleもexitもここで処理される）
			if (SystemMagic::Execute(spell))
			{
				chantSystem_->Cancel();
				return;
			}
			else
			{
				isError_ = true;
				errorTimer_ = 2.0f;
				chantSystem_->Start();
			}
		}
	}
	else
	{
		// TABキーで詠唱ウィンドウを開く
		if (ins.IsTrgDown(KEY_INPUT_TAB))
		{
			chantSystem_->Start();
			return;
		}

		// SPACEキーで即タイトルへ戻る
		if (ins.IsTrgDown(KEY_INPUT_SPACE))
		{
			SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::TITLE);
			return;
		}
	}
}

void ClearScene::DrawVictoryCircle(int cx, int cy, float radius, float angle, unsigned int color)
{
	DrawCircle(cx, cy, static_cast<int>(radius), color, FALSE);
	DrawCircle(cx, cy, static_cast<int>(radius * 0.85f), color, FALSE);

	// 八芒星（正方形×2）の祝福魔法陣
	for (int sq = 0; sq < 2; ++sq)
	{
		float baseAngle = angle + (sq * DX_PI_F / 4.0f);
		for (int i = 0; i < 4; ++i)
		{
			float a1 = baseAngle + (i * DX_PI_F / 2.0f);
			float a2 = baseAngle + ((i + 1) * DX_PI_F / 2.0f);

			int x1 = cx + static_cast<int>(cosf(a1) * radius * 0.85f);
			int y1 = cy + static_cast<int>(sinf(a1) * radius * 0.85f);
			int x2 = cx + static_cast<int>(cosf(a2) * radius * 0.85f);
			int y2 = cy + static_cast<int>(sinf(a2) * radius * 0.85f);

			DrawLine(x1, y1, x2, y2, color, 2);
		}
	}
}

void ClearScene::Draw(void)
{
	// 1. 3D背景と勝利ポーズのプレイヤーを描画
	skyDome_->Draw();
	MV1DrawModel(planetTransform_.modelId);
	MV1DrawModel(playerTransform_.modelId);

	int screenW = Application::GetScreenWidth();
	int screenH = Application::GetScreenHeight();
	UpdateFonts(screenH);

	int cx = screenW / 2;
	int cy = static_cast<int>(screenH * 0.28f);

	// 2. 黄金に輝く祝福の魔法陣を描画
	float circleRadius = screenH * 0.22f;
	SetDrawBlendMode(DX_BLENDMODE_ADD, 170);
	DrawVictoryCircle(cx, cy, circleRadius, animTime_ * 0.5f, GetColor(255, 200, 40));
	DrawVictoryCircle(cx, cy, circleRadius * 0.65f, -animTime_ * 0.7f, GetColor(255, 240, 120));
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	// 3. GAME CLEAR ロゴ
	std::string subText = "― 全ての脅威を討ち払った ―";
	std::string clearText = "QUEST CLEAR";

	int subW = GetDrawStringWidthToHandle(subText.c_str(), static_cast<int>(subText.length()), fontSub_);
	int clearW = GetDrawStringWidthToHandle(clearText.c_str(), static_cast<int>(clearText.length()), fontClear_);

	int logoY = static_cast<int>(screenH * 0.16f);
	DrawStringToHandle(cx - (subW / 2), logoY - static_cast<int>(screenH * 0.05f),
		subText.c_str(), GetColor(255, 240, 180), fontSub_, GetColor(60, 40, 0));

	DrawStringToHandle(cx - (clearW / 2), logoY,
		clearText.c_str(), GetColor(255, 235, 80), fontClear_, GetColor(140, 70, 0));

	// 4. 操作案内または詠唱ウィンドウ
	if (!chantSystem_->IsChanting())
	{
		int alpha = static_cast<int>(180.0f + 75.0f * sinf(animTime_ * 4.0f));
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);

		std::string guide1 = "Press [ SPACE ] to Return to Title";
		std::string guide2 = "Press [ TAB ] to Chant \"title\"";

		int g1W = GetDrawStringWidthToHandle(guide1.c_str(), static_cast<int>(guide1.length()), fontGuide_);
		int g2W = GetDrawStringWidthToHandle(guide2.c_str(), static_cast<int>(guide2.length()), fontGuide_);

		int guideY = static_cast<int>(screenH * 0.80f);
		DrawStringToHandle(cx - (g1W / 2), guideY, guide1.c_str(), GetColor(255, 255, 180), fontGuide_, GetColor(40, 30, 0));
		DrawStringToHandle(cx - (g2W / 2), guideY + static_cast<int>(screenH * 0.06f), guide2.c_str(), GetColor(0, 255, 255), fontGuide_, GetColor(0, 40, 60));

		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}
	else
	{
		if (isError_)
		{
			std::string errText = "呪文が違います！ 正しい呪文を詠唱してください";
			int errW = GetDrawStringWidthToHandle(errText.c_str(), static_cast<int>(errText.length()), fontSub_);
			DrawStringToHandle(cx - (errW / 2), static_cast<int>(screenH * 0.72f),
				errText.c_str(), GetColor(255, 80, 80), fontSub_, GetColor(40, 0, 0));
		}

		chantSystem_->Draw();
	}
}
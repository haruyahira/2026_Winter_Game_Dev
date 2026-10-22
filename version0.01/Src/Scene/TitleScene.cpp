// Src/Scene/TitleScene.cpp
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
#include "../Object/Charactor/Player/ChantSystem.h"
#include "TitleScene.h"

TitleScene::TitleScene(void)
	: skyDome_(nullptr)
	, chantSystem_(nullptr)
	, animTime_(0.0f)
	, isError_(false)
	, errorTimer_(0.0f)
	, fontTitle_(-1)
	, fontSub_(-1)
	, fontGuide_(-1)
	, lastScreenH_(0)
{
}

TitleScene::~TitleScene(void)
{
	if (fontTitle_ != -1) DeleteFontToHandle(fontTitle_);
	if (fontSub_ != -1) DeleteFontToHandle(fontSub_);
	if (fontGuide_ != -1) DeleteFontToHandle(fontGuide_);
}

void TitleScene::UpdateFonts(int screenH)
{
	if (screenH == lastScreenH_ && fontTitle_ != -1) return;

	if (fontTitle_ != -1) DeleteFontToHandle(fontTitle_);
	if (fontSub_ != -1) DeleteFontToHandle(fontSub_);
	if (fontGuide_ != -1) DeleteFontToHandle(fontGuide_);

	lastScreenH_ = screenH;

	int titleSize = static_cast<int>(screenH * 0.085f);
	int subSize = static_cast<int>(screenH * 0.030f);
	int guideSize = static_cast<int>(screenH * 0.032f);

	if (titleSize < 36) titleSize = 36;
	if (subSize < 16)   subSize = 16;
	if (guideSize < 18) guideSize = 18;

	fontTitle_ = CreateFontToHandle(NULL, titleSize, 6, DX_FONTTYPE_ANTIALIASING_EDGE_8X8);
	fontSub_ = CreateFontToHandle(NULL, subSize, 3, DX_FONTTYPE_ANTIALIASING_EDGE_4X4);
	fontGuide_ = CreateFontToHandle(NULL, guideSize, 3, DX_FONTTYPE_ANTIALIASING_EDGE_4X4);
}

void TitleScene::Init(void)
{
	// 定点カメラ
	mainCamera.ChangeMode(Camera::MODE::FIXED_POINT);

	// 背景のスカイドームと惑星モデルを配置
	planetTransform_.SetModel(resMng_.LoadModelDuplicate(ResourceManager::SRC::MAIN_PLANET));
	planetTransform_.scl = VScale(AsoUtility::VECTOR_ONE, 0.45f);
	planetTransform_.pos = { 0.0f, -150.0f, 350.0f };
	planetTransform_.quaRot = Quaternion();
	planetTransform_.Update();

	skyDome_ = std::make_unique<SkyDome>(planetTransform_);
	skyDome_->Init();

	// タイトル用の詠唱システム準備
	chantSystem_ = std::make_unique<ChantSystem>();
	chantSystem_->SetHintText("【Enter】詠唱発動 / 【Tab】閉じる （開始の呪文: GameStart）");
}

void TitleScene::Update(void)
{
	float deltaTime = SceneManager::GetInstance().GetDeltaTime();
	animTime_ += deltaTime;

	if (errorTimer_ > 0.0f)
	{
		errorTimer_ -= deltaTime;
		if (errorTimer_ <= 0.0f) isError_ = false;
	}

	// 背景の惑星をゆっくり自転させる
	planetTransform_.quaRot = planetTransform_.quaRot.Mult(
		Quaternion::AngleAxis(AsoUtility::Deg2RadD(8.0 * deltaTime), AsoUtility::AXIS_Y));
	planetTransform_.Update();
	skyDome_->Update();

	InputManager& ins = InputManager::GetInstance();

	// 詠唱モード中の処理
	if (chantSystem_->IsChanting())
	{
		// TABキーで詠唱キャンセル
		if (ins.IsTrgDown(KEY_INPUT_TAB))
		{
			chantSystem_->Cancel();
			return;
		}

		chantSystem_->Update();

		if (chantSystem_->IsFinished())
		{
			std::string spell = chantSystem_->GetTypedText();
			// 大文字・小文字やスペースの有無を吸収して判定（"GameStart", "gamestart", "game start" すべてOK）
			std::string normalized = "";
			for (char c : spell)
			{
				if (c != ' ') normalized += static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
			}

			if (normalized == "gamestart")
			{
				chantSystem_->Cancel();
				SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::GAME);
				return;
			}
			else
			{
				// 呪文が違う場合は再入力を促す
				isError_ = true;
				errorTimer_ = 2.0f;
				chantSystem_->Start();
			}
		}
	}
	else
	{
		// 詠唱していない時：TABキーで詠唱ウィンドウを開く
		if (ins.IsTrgDown(KEY_INPUT_TAB))
		{
			chantSystem_->Start();
			return;
		}

		// SPACEキーなら詠唱なしで即座にゲームスタート！
		if (ins.IsTrgDown(KEY_INPUT_SPACE))
		{
			SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::GAME);
			return;
		}
	}
}

void TitleScene::DrawMagicCircle(int cx, int cy, float radius, float angle, unsigned int color)
{
	// 外周の二重円
	DrawCircle(cx, cy, static_cast<int>(radius), color, FALSE);
	DrawCircle(cx, cy, static_cast<int>(radius * 0.88f), color, FALSE);
	DrawCircle(cx, cy, static_cast<int>(radius * 0.45f), color, FALSE);

	// 六芒星（正三角形×2）の頂点計算と描画
	for (int triangle = 0; triangle < 2; ++triangle)
	{
		float baseAngle = angle + (triangle * DX_PI_F / 3.0f);
		for (int i = 0; i < 3; ++i)
		{
			float a1 = baseAngle + (i * 2.0f * DX_PI_F / 3.0f);
			float a2 = baseAngle + ((i + 1) * 2.0f * DX_PI_F / 3.0f);

			int x1 = cx + static_cast<int>(cosf(a1) * radius * 0.88f);
			int y1 = cy + static_cast<int>(sinf(a1) * radius * 0.88f);
			int x2 = cx + static_cast<int>(cosf(a2) * radius * 0.88f);
			int y2 = cy + static_cast<int>(sinf(a2) * radius * 0.88f);

			DrawLine(x1, y1, x2, y2, color, 2);
		}
	}

	// 魔導リング上の光の節点
	for (int i = 0; i < 6; ++i)
	{
		float a = -angle * 1.5f + (i * DX_PI_F / 3.0f);
		int px = cx + static_cast<int>(cosf(a) * radius * 0.94f);
		int py = cy + static_cast<int>(sinf(a) * radius * 0.94f);
		DrawCircle(px, py, 5, GetColor(180, 255, 255), TRUE);
	}
}

void TitleScene::Draw(void)
{
	// 1. 3D背景（宇宙＆惑星）の描画
	skyDome_->Draw();
	MV1DrawModel(planetTransform_.modelId);

	int screenW = Application::GetScreenWidth();
	int screenH = Application::GetScreenHeight();
	UpdateFonts(screenH);

	// 画面全体を少し落ち着いたトーンにするグラデーション幕
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 90);
	DrawBox(0, 0, screenW, screenH, GetColor(5, 10, 25), TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	int cx = screenW / 2;
	int cy = static_cast<int>(screenH * 0.42f);

	// 2. 回転する多重魔法陣の描画
	float circleRadius = screenH * 0.28f;
	SetDrawBlendMode(DX_BLENDMODE_ADD, 160);
	DrawMagicCircle(cx, cy, circleRadius, animTime_ * 0.4f, GetColor(0, 180, 255));
	DrawMagicCircle(cx, cy, circleRadius * 0.65f, -animTime_ * 0.6f, GetColor(80, 220, 255));
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	// 3. タイトルロゴ（美しいアンチエイリアス＋発光エッジ）
	std::string subTitle = "― 詠唱魔法アクション ―";
	std::string mainTitle = "CHANT ARCANUM";

	int subW = GetDrawStringWidthToHandle(subTitle.c_str(), static_cast<int>(subTitle.length()), fontSub_);
	int mainW = GetDrawStringWidthToHandle(mainTitle.c_str(), static_cast<int>(mainTitle.length()), fontTitle_);

	int titleY = static_cast<int>(screenH * 0.28f);
	DrawStringToHandle(cx - (subW / 2), titleY - static_cast<int>(screenH * 0.05f),
		subTitle.c_str(), GetColor(180, 240, 255), fontSub_, GetColor(0, 30, 60));

	DrawStringToHandle(cx - (mainW / 2), titleY,
		mainTitle.c_str(), GetColor(255, 255, 255), fontTitle_, GetColor(0, 120, 220));

	// 4. 操作案内（詠唱していない時は点滅ガイドを表示）
	if (!chantSystem_->IsChanting())
	{
		int alpha = static_cast<int>(180.0f + 75.0f * sinf(animTime_ * 4.0f));
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);

		std::string guide1 = "Press [ SPACE ] to Quick Start";
		std::string guide2 = "Press [ TAB ] to Chant \"GameStart\"";

		int g1W = GetDrawStringWidthToHandle(guide1.c_str(), static_cast<int>(guide1.length()), fontGuide_);
		int g2W = GetDrawStringWidthToHandle(guide2.c_str(), static_cast<int>(guide2.length()), fontGuide_);

		int guideY = static_cast<int>(screenH * 0.76f);
		DrawStringToHandle(cx - (g1W / 2), guideY, guide1.c_str(), GetColor(255, 255, 150), fontGuide_, GetColor(40, 30, 0));
		DrawStringToHandle(cx - (g2W / 2), guideY + static_cast<int>(screenH * 0.06f), guide2.c_str(), GetColor(0, 255, 255), fontGuide_, GetColor(0, 40, 60));

		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}
	else
	{
		// 呪文間違い時のエラーメッセージ
		if (isError_)
		{
			std::string errText = "呪文が違います！ \"GameStart\" と詠唱してください";
			int errW = GetDrawStringWidthToHandle(errText.c_str(), static_cast<int>(errText.length()), fontSub_);
			DrawStringToHandle(cx - (errW / 2), static_cast<int>(screenH * 0.72f),
				errText.c_str(), GetColor(255, 80, 80), fontSub_, GetColor(40, 0, 0));
		}

		// 画面下部の詠唱ウィンドウを描画
		chantSystem_->Draw();
	}
}
#include <chrono>
#include <DxLib.h>
#include <EffekseerForDXLib.h>
#include "../Common/Fader.h"
#include "../Scene/TitleScene.h"
#include "../Scene/GameScene.h"
#include "../Scene/ClearScene.h"
#include "Camera.h"
#include "ResourceManager.h"
#include "SceneManager.h"
#include "InputManager.h"

SceneManager* SceneManager::instance_ = nullptr;

void SceneManager::CreateInstance()
{
	if (instance_ == nullptr)
	{
		instance_ = new SceneManager();
	}
	instance_->Init();
}

SceneManager& SceneManager::GetInstance(void)
{
	return *instance_;
}

SceneManager::SceneManager(void)
{
	sceneId_ = SCENE_ID::NONE;
	waitSceneId_ = SCENE_ID::NONE;

	scene_ = nullptr;
	fader_ = nullptr;

	isSceneChanging_ = false;

	// デルタタイム
	deltaTime_ = 1.0f / 60.0f;

	camera_ = nullptr;
	freeCamera_ = nullptr;
	isFreeCameraMode_ = false;
}

void SceneManager::Init(void)
{
	sceneId_ = SCENE_ID::GAME;
	waitSceneId_ = SCENE_ID::NONE;

	fader_ = std::make_unique<Fader>();
	fader_->Init();

	// プレイヤー用メインカメラの初期化
	camera_ = std::make_unique<Camera>();
	camera_->Init();
	camera_->SetActive(true); // 最初はこっちをON

	// 独立したフリーカメラの初期化
	freeCamera_ = std::make_unique<Camera>();
	freeCamera_->Init();
	freeCamera_->ChangeMode(Camera::MODE::FREE);
	freeCamera_->SetActive(false); // 最初はOFFにしておく

	isFreeCameraMode_ = false;

	isSceneChanging_ = false;

	// デルタタイム
	preTime_ = std::chrono::system_clock::now();

	// 3D用の設定
	Init3D();

	// 初期シーンの設定
	DoChangeScene(SCENE_ID::GAME);
}

void SceneManager::Init3D(void)
{
	// 背景色設定
	SetBackgroundColor(0, 139, 139);

	// Zバッファを有効にする
	SetUseZBuffer3D(true);

	// Zバッファへの書き込みを有効にする
	SetWriteZBuffer3D(true);

	// バックカリングを有効にする
	SetUseBackCulling(true);

	// ライトの設定
	SetUseLighting(true);

	SetFogEnable(false);
	SetFogColor(0, 0, 0); // 修正：フォグの色を 5, 5, 5 から 0, 0, 0 に
	SetFogStartEnd(1000.0f, 5000.0f); // 暗闇を演出するため距離を少し短くする
}

void SceneManager::Update(void)
{
	if (scene_ == nullptr)
	{
		return;
	}

	if (InputManager::GetInstance().IsTrgDown(KEY_INPUT_3))
	{
		isFreeCameraMode_ = !isFreeCameraMode_;

		if (isFreeCameraMode_)
		{
			// フリーカメラON
			camera_->SetActive(false);
			freeCamera_->SetActive(true);

			// 切り替えた瞬間、今のプレイヤー視点の位置から幽体離脱させる
			freeCamera_->SetPos(camera_->GetPos());
			freeCamera_->SetAngles(camera_->GetAngles());
		}
		else
		{
			// メインカメラ(プレイヤー視点)に戻す
			camera_->SetActive(true);
			freeCamera_->SetActive(false);
		}
	}

	// デルタタイム
	auto nowTime = std::chrono::system_clock::now();
	deltaTime_ = static_cast<float>(
		std::chrono::duration_cast<std::chrono::nanoseconds>(nowTime - preTime_).count() / 1000000000.0);
	preTime_ = nowTime;

	fader_->Update();
	if (isSceneChanging_)
	{
		Fade();
	}
	else
	{
		scene_->Update();
	}

	// カメラ更新
	camera_->Update();
	freeCamera_->Update();
}

void SceneManager::Draw(void)
{
	// 描画先グラフィック領域の指定
	// (３Ｄ描画で使用するカメラの設定などがリセットされる)
	SetDrawScreen(DX_SCREEN_BACK);

	// 画面を初期化
	ClearDrawScreen();

	// カメラ設定
	if (isFreeCameraMode_) {
		freeCamera_->SetBeforeDraw();
	}
	else {
		camera_->SetBeforeDraw();
	}

	// Effekseerにより再生中のエフェクトを更新する。
	UpdateEffekseer3D();

	// 描画
	scene_->Draw();

	// 主にポストエフェクト用
	camera_->Draw();

	// Effekseerにより再生中のエフェクトを描画する。
	DrawEffekseer3D();
	
	// 暗転・明転
	fader_->Draw();
}

void SceneManager::Destroy(void)
{
	delete instance_;
}

void SceneManager::ChangeScene(SCENE_ID nextId)
{
	// フェード処理が終わってからシーンを変える場合もあるため、
	// 遷移先シーンをメンバ変数に保持
	waitSceneId_ = nextId;

	// フェードアウト(暗転)を開始する
	fader_->SetFade(Fader::STATE::FADE_OUT);
	isSceneChanging_ = true;
}

SceneManager::SCENE_ID SceneManager::GetSceneID(void)
{
	return sceneId_;
}

float SceneManager::GetDeltaTime(void) const
{
	//return 1.0f / 60.0f;
	return deltaTime_;
}

Camera& SceneManager::GetCamera(void)
{
	return *camera_;
}

void SceneManager::ResetDeltaTime(void)
{
	deltaTime_ = 0.016f;
	preTime_ = std::chrono::system_clock::now();
}

void SceneManager::DoChangeScene(SCENE_ID sceneId)
{
	// リソースの解放
	ResourceManager::GetInstance().Release();

	// シーンを変更する
	sceneId_ = sceneId;

	// 現在のシーンを解放
	if (scene_ != nullptr)
	{
		scene_.reset();
	}

	switch (sceneId_)
	{
	case SCENE_ID::TITLE:
		scene_ = std::make_unique<TitleScene>();
		break;
	case SCENE_ID::GAME:
		scene_ = std::make_unique<GameScene>();
		break;
	case SCENE_ID::CLEAR:
		scene_ = std::make_unique<ClearScene>();
		break;
	}

	scene_->Init();

	ResetDeltaTime();

	waitSceneId_ = SCENE_ID::NONE;
}

void SceneManager::Fade(void)
{
	Fader::STATE fState = fader_->GetState();
	switch (fState)
	{
	case Fader::STATE::FADE_IN:
		// 明転中
		if (fader_->IsEnd())
		{
			// 明転が終了したら、フェード処理終了
			fader_->SetFade(Fader::STATE::NONE);
			isSceneChanging_ = false;
		}
		break;
	case Fader::STATE::FADE_OUT:
		// 暗転中
		if (fader_->IsEnd())
		{
			// 完全に暗転してからシーン遷移
			DoChangeScene(waitSceneId_);
			// 暗転から明転へ
			fader_->SetFade(Fader::STATE::FADE_IN);
		}
		break;
	}
}



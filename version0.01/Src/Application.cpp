// Src/Application.cpp
#include <DxLib.h>
#include <EffekseerForDXLib.h>
#include "Manager/InputManager.h"
#include "Manager/ResourceManager.h"
#include "Manager/SceneManager.h"
#include "Application.h"

Application* Application::instance_ = nullptr;
int Application::screenWidth_ = Application::SCREEN_SIZE_X;
int Application::screenHeight_ = Application::SCREEN_SIZE_Y;

const std::string Application::PATH_IMAGE = "Data/Image/";
const std::string Application::PATH_MODEL = "Data/Model/";
const std::string Application::PATH_EFFECT = "Data/Effect/";
const std::string Application::PATH_MAGIC = "Data/Magic/";
const std::string Application::PATH_SHADER = "Data/Shader/";

void Application::CreateInstance(void)
{
	if (instance_ == nullptr)
	{
		instance_ = new Application();
	}
	instance_->Init();
}

Application& Application::GetInstance(void)
{
	return *instance_;
}

void Application::Init(void)
{
	// アプリケーションの初期設定
	SetWindowText("3DAction");

	// ★ お使いのパソコンのモニター解像度を自動取得してフルスクリーンに設定
	int desktopW = 1920;
	int desktopH = 1080;
	int colorBit = 32;
	if (GetDefaultState(&desktopW, &desktopH, &colorBit) == 0 && desktopW > 0 && desktopH > 0)
	{
		screenWidth_ = desktopW;
		screenHeight_ = desktopH;
	}
	else
	{
		screenWidth_ = GetSystemMetrics(SM_CXSCREEN);
		screenHeight_ = GetSystemMetrics(SM_CYSCREEN);
		if (screenWidth_ <= 0) screenWidth_ = 1920;
		if (screenHeight_ <= 0) screenHeight_ = 1080;
	}

	SetGraphMode(screenWidth_, screenHeight_, 32);
	ChangeWindowMode(false); // フルスクリーン化

	// DxLibの初期化
	SetUseDirect3DVersion(DX_DIRECT3D_11);
	isInitFail_ = false;
	if (DxLib_Init() == -1)
	{
		isInitFail_ = true;
		return;
	}

	// マウスカーソルを非表示にし、画面中央に初期配置
	SetMouseDispFlag(FALSE);
	SetMousePoint(screenWidth_ / 2, screenHeight_ / 2);

	// Effekseerの初期化
	InitEffekseer();

	// キー制御初期化
	SetUseDirectInputFlag(true);
	InputManager::CreateInstance();

	// リソース管理初期化
	ResourceManager::CreateInstance();

	// シーン管理初期化
	SceneManager::CreateInstance();
}

void Application::Run(void)
{
	auto& inputManager = InputManager::GetInstance();
	auto& sceneManager = SceneManager::GetInstance();

	// ゲームループ
	while (ProcessMessage() == 0 && CheckHitKey(KEY_INPUT_ESCAPE) == 0)
	{
		inputManager.Update();
		sceneManager.Update();

		sceneManager.Draw();

		ScreenFlip();
	}
}

void Application::Destroy(void)
{
	InputManager::GetInstance().Destroy();
	ResourceManager::GetInstance().Destroy();
	SceneManager::GetInstance().Destroy();

	// Effekseerを終了する。
	Effkseer_End();

	// DxLib終了
	if (DxLib_End() == -1)
	{
		isReleaseFail_ = true;
	}

	delete instance_;
}

bool Application::IsInitFail(void) const
{
	return isInitFail_;
}

bool Application::IsReleaseFail(void) const
{
	return isReleaseFail_;
}

Application::Application(void)
{
	isInitFail_ = false;
	isReleaseFail_ = false;
}

void Application::InitEffekseer(void)
{
	if (Effekseer_Init(8000) == -1)
	{
		DxLib_End();
	}

	SetChangeScreenModeGraphicsSystemResetFlag(FALSE);

	Effekseer_SetGraphicsDeviceLostCallbackFunctions();
}
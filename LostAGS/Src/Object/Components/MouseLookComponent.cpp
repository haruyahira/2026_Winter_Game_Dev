// MouseLookComponent.cpp
#include "MouseLookComponent.h"
#include "../ActorBase.h"
#include "../../Manager/InputManager.h"
#include "../../Manager/Camera.h"
#include "../../Manager/SceneManager.h"
#include "../../Utility/AsoUtility.h"

MouseLookComponent::MouseLookComponent(void)
	: playerRotY_(Quaternion())
{
}

void MouseLookComponent::Update(void)
{
	if (!owner_) return;

	auto& input = InputManager::GetInstance();
	Vector2 mousePos = input.GetMousePos();

	// 画面の中央座標を基準にする
	int screenW, screenH;
	GetScreenState(&screenW, &screenH, nullptr);
	int centerX = screenW / 2;
	int centerY = screenH / 2;

	// マウスカーソルを常に画面中央にロックして360度無限に回せるようにする
	SetMousePoint(centerX, centerY);

	// マウスの移動量（中心からのズレ）を計算
	int deltaX = mousePos.x - centerX;

	if (deltaX != 0)
	{
		float deltaTime = SceneManager::GetInstance().GetDeltaTime();
		// マウス感度（必要に応じて調整してください）
		float sensitivity = 2.0f;

		// カメラの向き（Y軸方向）を基準に、マウスの横移動分だけプレイヤーの向きを回転させる
		VECTOR camAngles = mainCamera.GetAngles();
		double targetYaw = camAngles.y + (deltaX * sensitivity * deltaTime);

		playerRotY_ = Quaternion::AngleAxis(targetYaw, AsoUtility::AXIS_Y);
		owner_->transform_.quaRot = playerRotY_;
	}
}
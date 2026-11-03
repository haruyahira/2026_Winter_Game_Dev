// Src/Manager/Camera.cpp
#include <math.h>
#include <DxLib.h>
#include <EffekseerForDXLib.h>
#include "../Application.h"
#include "../Utility/AsoUtility.h"
#include "../Manager/InputManager.h"
#include "../Manager/SceneManager.h"
#include "../Object/Common/Transform.h"
#include "Camera.h"

Camera::Camera(void)
{
	angles_ = VECTOR();
	cameraUp_ = VECTOR();
	mode_ = MODE::NONE;
	pos_ = AsoUtility::VECTOR_ZERO;
	targetPos_ = AsoUtility::VECTOR_ZERO;
	followTransform_ = nullptr;
	isMouseInitialized_ = false;
	isActive_ = true; // ★追加：最初はONにしておく
}

Camera::~Camera(void)
{
}

void Camera::Init(void)
{
	ChangeMode(MODE::FIXED_POINT);
}

void Camera::Update(void)
{
	if (mode_ == MODE::FOLLOW && followTransform_ != nullptr)
	{
		// ★ isActive_ が true の時だけマウスで視点移動できる
		if (isActive_) ProcessRot();

		// ★ 追従処理は常に実行！（これでフリーカメラ中もプレイヤーが正常に動ける）
		SyncFollow();
	}
	else if (mode_ == MODE::FREE)
	{
		if (isActive_) // ★ フリーカメラもアクティブな時だけ動く
		{
			ProcessRot();
			ProcessFreeMove();
		}
	}
}

void Camera::SetBeforeDraw(void)
{
	// クリップ距離を設定する(SetDrawScreenでリセットされる)
	SetCameraNearFar(CAMERA_NEAR, CAMERA_FAR);

	switch (mode_)
	{
	case Camera::MODE::FIXED_POINT:
		SetBeforeDrawFixedPoint();
		break;
	case Camera::MODE::FOLLOW:
		SetBeforeDrawFollow();
		break;
	case Camera::MODE::FREE:          // ★ 追加
		SetBeforeDrawFree();          // ★ 追加
		break;                        // ★ 追加
	}

	// カメラの設定(位置と注視点による制御)
	SetCameraPositionAndTargetAndUpVec(
		pos_,
		targetPos_,
		cameraUp_
	);

	// DXライブラリのカメラとEffekseerのカメラを同期する。
	Effekseer_Sync3DSetting();
}

void Camera::Draw(void)
{
}

void Camera::SetFollow(const Transform* follow)
{
	followTransform_ = follow;
}

VECTOR Camera::GetPos(void) const
{
	return pos_;
}

VECTOR Camera::GetAngles(void) const
{
	return angles_;
}

VECTOR Camera::GetTargetPos(void) const
{
	return targetPos_;
}

Quaternion Camera::GetQuaRot(void) const
{
	return rot_;
}

Quaternion Camera::GetQuaRotOutX(void) const
{
	return rotOutX_;
}

VECTOR Camera::GetForward(void) const
{
	return VNorm(VSub(targetPos_, pos_));
}

void Camera::ChangeMode(MODE mode)
{
	// カメラの初期設定
	SetDefault();

	// カメラモードの変更
	mode_ = mode;
	isMouseInitialized_ = false;

	// 変更時の初期化処理
	switch (mode_)
	{
	case Camera::MODE::FIXED_POINT:
		break;
	case Camera::MODE::FOLLOW:
		if (followTransform_ != nullptr)
		{
			SyncFollow();
		}
		break;
	case Camera::MODE::FREE:
		// フリーカメラに切り替わった瞬間に一回だけ向きを更新しておく
		ProcessFreeMove();
		break;
	}
}

Camera::MODE Camera::GetMode(void) const
{
	return mode_;
}

void Camera::SetDefault(void)
{
	// カメラの初期設定
	pos_ = DEFAULT_CAMERA_POS;

	// 注視点
	targetPos_ = AsoUtility::VECTOR_ZERO;

	// カメラの上方向
	cameraUp_ = AsoUtility::DIR_U;

	angles_.x = AsoUtility::Deg2RadF(15.0f);
	angles_.y = 0.0f;
	angles_.z = 0.0f;

	rotOutX_ = Quaternion();
	rot_ = Quaternion();
}

void Camera::SyncFollow(void)
{
	if (!followTransform_) return;

	// プレイヤーの位置（足元やモデルの中心点）
	VECTOR pos = followTransform_->pos;

	// 正面から設定されたY軸分、回転させる
	rotOutX_ = Quaternion::AngleAxis(angles_.y, AsoUtility::AXIS_Y);

	// 正面から設定されたX軸分、回転させる
	rot_ = rotOutX_.Mult(Quaternion::AngleAxis(angles_.x, AsoUtility::AXIS_X));

	// カメラ位置の決定（一人称視点）
	// プレイヤーの基準点から「目の高さ」までカメラを持ち上げます。
	// ※ 15.0f の部分はご自身のゲームのキャラクタースケールに合わせて調整してください。
	VECTOR eyeOffset = VGet(0.0f, 150.0f, 0.0f);
	pos_ = VAdd(pos, eyeOffset);

	// 注視点の決定（向いている方向）
	// Z軸プラス方向（前方）ベクトルを、マウスで計算した回転（rot_）で回します。
	VECTOR forwardVec = VGet(0.0f, 0.0f, 100.0f); // 100.0f 先を注視点とする
	VECTOR localTarget = rot_.PosAxis(forwardVec);

	// カメラの位置（pos_）に、回転させた前方ベクトルを足して注視点にします。
	targetPos_ = VAdd(pos_, localTarget);

	// カメラの上方向
	cameraUp_ = AsoUtility::DIR_U;
}

void Camera::ProcessRot(void)
{
	// ウィンドウがアクティブでない時はマウスをロックしない
	if (GetMainWindowHandle() != GetForegroundWindow())
	{
		isMouseInitialized_ = false;
		return;
	}

	int centerX = Application::GetScreenWidth() / 2;
	int centerY = Application::GetScreenHeight() / 2;

	// 初回フレームはマウスを中央にセットするだけにして跳ねを防ぐ
	if (!isMouseInitialized_)
	{
		SetMousePoint(centerX, centerY);
		isMouseInitialized_ = true;
		return;
	}

	int mouseX = centerX;
	int mouseY = centerY;
	GetMousePoint(&mouseX, &mouseY);

	int deltaX = mouseX - centerX;
	int deltaY = mouseY - centerY;

	// マウスを再び画面中央へ戻す（これで360度無限に回せる）
	SetMousePoint(centerX, centerY);

	// ★ マウス移動量でカメラを360度自由回転
	angles_.y += static_cast<float>(deltaX) * MOUSE_SENSITIVITY;
	angles_.x += static_cast<float>(deltaY) * MOUSE_SENSITIVITY;

	// 上下の角度制限
	if (angles_.x > LIMIT_X_UP_RAD)
	{
		angles_.x = LIMIT_X_UP_RAD;
	}
	if (angles_.x < -LIMIT_X_DW_RAD)
	{
		angles_.x = -LIMIT_X_DW_RAD;
	}
}

void Camera::ProcessFreeMove(void)
{
	// 1. 現在のカメラの回転（rot_）を更新
	rotOutX_ = Quaternion::AngleAxis(angles_.y, AsoUtility::AXIS_Y);
	rot_ = rotOutX_.Mult(Quaternion::AngleAxis(angles_.x, AsoUtility::AXIS_X));

	// 2. カメラから見た「前」「右」「上」のベクトルを取得
	VECTOR forward = rot_.PosAxis(VGet(0.0f, 0.0f, 1.0f));
	VECTOR right = rot_.PosAxis(VGet(1.0f, 0.0f, 0.0f));
	VECTOR up = rot_.PosAxis(VGet(0.0f, 1.0f, 0.0f));

	// 3. 入力による移動方向の計算
	auto& ins = InputManager::GetInstance();
	VECTOR moveDir = AsoUtility::VECTOR_ZERO;

	if (ins.IsNew(KEY_INPUT_W)) moveDir = VAdd(moveDir, forward);
	if (ins.IsNew(KEY_INPUT_S)) moveDir = VAdd(moveDir, VScale(forward, -1.0f));
	if (ins.IsNew(KEY_INPUT_D)) moveDir = VAdd(moveDir, right);
	if (ins.IsNew(KEY_INPUT_A)) moveDir = VAdd(moveDir, VScale(right, -1.0f));
	if (ins.IsNew(KEY_INPUT_E)) moveDir = VAdd(moveDir, AsoUtility::DIR_U); // Eで上昇
	if (ins.IsNew(KEY_INPUT_Q)) moveDir = VAdd(moveDir, VScale(AsoUtility::DIR_U, -1.0f)); // Qで下降

	// 4. カメラの座標を更新
	if (!AsoUtility::EqualsVZero(moveDir))
	{
		moveDir = VNorm(moveDir);

		// 基本スピードと、Shiftキー押しっぱなしによるダッシュ
		float speed = 15.0f;
		if (ins.IsNew(KEY_INPUT_LSHIFT) || ins.IsNew(KEY_INPUT_RSHIFT))
		{
			speed *= 4.0f; // ダッシュ時は4倍速
		}

		float dtScale = SceneManager::GetInstance().GetDeltaTime() * 60.0f;
		pos_ = VAdd(pos_, VScale(moveDir, speed * dtScale));
	}

	// 5. 注視点（カメラの少し前方の位置）を更新
	targetPos_ = VAdd(pos_, VScale(forward, 100.0f));
	cameraUp_ = AsoUtility::DIR_U;
}

void Camera::SetBeforeDrawFixedPoint(void)
{
}

void Camera::SetBeforeDrawFollow(void)
{
	// 描画直前にも位置同期を確実に行う
	SyncFollow();
}

void Camera::SetBeforeDrawSelfShot(void)
{
}

void Camera::SetBeforeDrawFree(void)
{
	ProcessFreeMove();
}
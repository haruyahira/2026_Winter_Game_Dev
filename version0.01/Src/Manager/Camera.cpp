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
	// ★ Updateのタイミングでカメラ回転と同期を行うことで、
	// プレイヤーとカメラの向きが1フレームのズレもなく完全に一致します！
	if (mode_ == MODE::FOLLOW && followTransform_ != nullptr)
	{
		ProcessRot();
		SyncFollow();
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
	}
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

	// ---------------------------------------------------
	// ① カメラ位置の決定（一人称視点）
	// ---------------------------------------------------
	// プレイヤーの基準点から「目の高さ」までカメラを持ち上げます。
	// ※ 15.0f の部分はご自身のゲームのキャラクタースケールに合わせて調整してください。
	VECTOR eyeOffset = VGet(0.0f, 100.0f, 0.0f);
	pos_ = VAdd(pos, eyeOffset);

	// ---------------------------------------------------
	// ② 注視点の決定（向いている方向）
	// ---------------------------------------------------
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

	// 矢印キーでも補助的に回せるようにしておく（FPS非依存）
	auto& ins = InputManager::GetInstance();
	float dtScale = SceneManager::GetInstance().GetDeltaTime() * 60.0f;
	if (ins.IsNew(KEY_INPUT_RIGHT)) { angles_.y += AsoUtility::Deg2RadF(1.5f * dtScale); }
	if (ins.IsNew(KEY_INPUT_LEFT)) { angles_.y += AsoUtility::Deg2RadF(-1.5f * dtScale); }
	if (ins.IsNew(KEY_INPUT_UP)) { angles_.x += AsoUtility::Deg2RadF(1.5f * dtScale); }
	if (ins.IsNew(KEY_INPUT_DOWN)) { angles_.x += AsoUtility::Deg2RadF(-1.5f * dtScale); }

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
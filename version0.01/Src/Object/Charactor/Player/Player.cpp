#include <string>
#include "../../../Application.h"
#include "../../../Utility/AsoUtility.h"
#include "../../../Manager/InputManager.h"
#include "../../../Manager/SceneManager.h"
#include "../../../Manager/ResourceManager.h"
#include "../../../Manager/Camera.h"
#include "../../../Renderer/ModelMaterial.h"
#include "../../../Renderer/ModelRenderer.h"
#include "../../Common/AnimationController.h"
#include "../../Common/Capsule.h"
#include "../../Common/Collider.h"
#include "../../Planet.h"
#include "Player.h"

Player::Player(void)
{
	state_ = STATE::NONE;

	playerRotY_ = Quaternion();
	goalQuaRot_ = Quaternion();
	stepRotTime_ = 0.0f;
	stepJump_ = 0.0f;

	stateChanges_.emplace(STATE::NONE, std::bind(&Player::ChangeStateNone, this));
	stateChanges_.emplace(STATE::PLAY, std::bind(&Player::ChangeStatePlay, this));
}

Player::~Player(void) 
{
}

void Player::InitDetail(void)
{
	auto& resMng_ = ResourceManager::GetInstance();

	// モデル(リソースID, スケール, 座標, ローカル軸回転)
	LoadAndSetupModel(
		ResourceManager::SRC::PLAYER,
		INIT_SCALE, 
		INIT_POSITION, 
		INIT_AXIS);

	// 当たり判定(上端Y, 下端Y, 半径)
	SetupCapsule(CAPSULE_TOP, CAPSULE_BOTTOM, CAPSULE_RADIUS);

	// アニメーションの準備
	InitAnimation();

	// 初期状態
	ChangeState(STATE::PLAY);
}

void Player::InitAnimation(void)
{
	std::string path = Application::PATH_MODEL + "Player/";
	animationController_ = std::make_unique<AnimationController>(transform_.modelId);
	animationController_->Add((int)ANIM_TYPE::IDLE, path + "Idle.mv1", 20.0f);
	animationController_->Add((int)ANIM_TYPE::RUN, path + "Run.mv1", 20.0f);
	animationController_->Add((int)ANIM_TYPE::FAST_RUN, path + "FastRun.mv1", 20.0f);
	animationController_->Add((int)ANIM_TYPE::JUMP, path + "Jump.mv1", 60.0f);
	animationController_->Add((int)ANIM_TYPE::WARP_PAUSE, path + "WarpPose.mv1", 60.0f);
	animationController_->Add((int)ANIM_TYPE::FLY, path + "Flying.mv1", 60.0f);
	animationController_->Add((int)ANIM_TYPE::FALLING, path + "Falling.mv1", 80.0f);
	animationController_->Add((int)ANIM_TYPE::VICTORY, path + "Victory.mv1", 60.0f);

	animationController_->Play((int)ANIM_TYPE::IDLE);
}


void Player::Update(void)
{
	stateUpdate_();
	transform_.Update();
	animationController_->Update();
}

void Player::Draw(void)
{
	if (material_ && renderer_)
	{
		auto& camera = SceneManager::GetInstance().GetCamera();
		auto camaraPos = camera.GetPos();
		material_->SetConstBufVS(0, { camaraPos.x, camaraPos.y, camaraPos.z, 0.0f });

		renderer_->Draw();
	}

	// 丸影の描画
	DrawShadow();
}

void Player::ChangeState(STATE state)
{
	state_ = state;
	stateChanges_[state_]();
}

void Player::ChangeStateNone(void) 
{ 
	stateUpdate_ = std::bind(&Player::UpdateNone, this); 
}

void Player::ChangeStatePlay(void) 
{
	stateUpdate_ = std::bind(&Player::UpdatePlay, this); 
}

void Player::UpdateNone(void) 
{
}

void Player::UpdatePlay(void)
{
	ProcessMove();
	ProcessJump();
	Rotate();
	CalcGravityPow();
	Collision();

	transform_.quaRot = playerRotY_;
}

void Player::ProcessMove(void)
{
	auto& ins = InputManager::GetInstance();
	movePow_ = AsoUtility::VECTOR_ZERO;

	Quaternion cameraRot = mainCamera.GetQuaRotOutX();
	double rotRad = 0;
	VECTOR dir = AsoUtility::VECTOR_ZERO;

	if (ins.IsNew(KEY_INPUT_W)) { rotRad = AsoUtility::Deg2RadD(0.0);   dir = cameraRot.GetForward(); }
	if (ins.IsNew(KEY_INPUT_S)) { rotRad = AsoUtility::Deg2RadD(180.0); dir = cameraRot.GetBack(); }
	if (ins.IsNew(KEY_INPUT_D)) { rotRad = AsoUtility::Deg2RadD(90.0);  dir = cameraRot.GetRight(); }
	if (ins.IsNew(KEY_INPUT_A)) { rotRad = AsoUtility::Deg2RadD(270.0); dir = cameraRot.GetLeft(); }

	if (!AsoUtility::EqualsVZero(dir) && (isJump_ || IsEndLanding())) {
		speed_ = ins.IsNew(KEY_INPUT_RSHIFT) ? SPEED_RUN : SPEED_MOVE;
		moveDir_ = dir;
		movePow_ = VScale(dir, speed_);
		SetGoalRotate(rotRad);

		if (!isJump_ && IsEndLanding()) {
			animationController_->Play(ins.IsNew(KEY_INPUT_RSHIFT) ? (int)ANIM_TYPE::FAST_RUN : (int)ANIM_TYPE::RUN);
		}
	}
	else {
		if (!isJump_ && IsEndLanding()) {
			animationController_->Play((int)ANIM_TYPE::IDLE);
		}
	}
}

void Player::ProcessJump(void)
{
	auto& scnMng_ = SceneManager::GetInstance();
	bool isHit = CheckHitKey(KEY_INPUT_SPACE);

	if (isHit && (isJump_ || IsEndLanding())) {
		if (!isJump_) {
			animationController_->Play((int)ANIM_TYPE::JUMP, true, ANIM_JUMP_START, ANIM_JUMP_LOOP_END);
			animationController_->SetEndLoop(ANIM_JUMP_LOOP_START, ANIM_JUMP_LOOP_END, ANIM_JUMP_BLEND);
		}
		isJump_ = true;
		stepJump_ += scnMng_.GetDeltaTime();
		if (stepJump_ < TIME_JUMP_IN) {
			jumpPow_ = VScale(AsoUtility::DIR_U, POW_JUMP);
		}
	}

	if (!isHit) {
		stepJump_ = TIME_JUMP_IN;
	}
}

void Player::SetGoalRotate(double rotRad)
{
	VECTOR cameraRot = mainCamera.GetAngles();
	Quaternion axis = Quaternion::AngleAxis((double)cameraRot.y + rotRad, AsoUtility::AXIS_Y);

	if (Quaternion::Angle(axis, goalQuaRot_) > 0.1) {
		stepRotTime_ = TIME_ROT;
	}
	goalQuaRot_ = axis;
}

void Player::Rotate(void)
{
	auto& scnMng_ = SceneManager::GetInstance();
	stepRotTime_ -= scnMng_.GetDeltaTime();
	playerRotY_ = Quaternion::Slerp(playerRotY_, goalQuaRot_, (TIME_ROT - stepRotTime_) / TIME_ROT);
}

// CharactorBaseから着地時に呼び出されるフック処理
void Player::OnLanding(void)
{
	stepJump_ = 0.0f;
	if (isJump_) {
		animationController_->Play((int)ANIM_TYPE::JUMP, false, ANIM_LANDING_START, ANIM_LANDING_END, false, true);
	}
	isJump_ = false;
}

bool Player::IsEndLanding(void)
{
	if (animationController_->GetPlayType() != (int)ANIM_TYPE::JUMP) return true;
	if (animationController_->IsEnd()) return true;
	return false;
}
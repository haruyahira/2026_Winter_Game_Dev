#include "BossAIComponent.h"
#include "../Charactor/CharactorBase.h"
#include "../../Manager/SceneManager.h"
#include "../../Utility/AsoUtility.h"
#include "../Common/AnimationController.h"
#include "../Common/Collider.h"
#include "../Planet.h"

BossAIComponent::BossAIComponent()
{
	state_ = STATE::NONE;
	step_ = 0.0f;
	moveDir_ = AsoUtility::VECTOR_ZERO;
}

void BossAIComponent::Update()
{
	// 本体（箱）を CharactorBase として取得する
	auto character = dynamic_cast<CharactorBase*>(owner_);
	if (!character) return;

	// 最初の一回だけ MOVE 状態にする
	if (state_ == STATE::NONE)
	{
		ChangeState(STATE::MOVE, character);
	}

	switch (state_)
	{
	case STATE::NONE:   break;
	case STATE::THINK:  UpdateThink(character); break;
	case STATE::IDLE:   UpdateIdle(character); break;
	case STATE::MOVE:   UpdateMove(character); break;
	case STATE::ACTION: UpdateAction(character); break;
	case STATE::END:    break;
	default:            break;
	}
}

void BossAIComponent::ChangeState(STATE state, CharactorBase* character)
{
	state_ = state;
	step_ = 0.0f;
	character->movePow_ = AsoUtility::VECTOR_ZERO; // 親クラスの変数にアクセス

	switch (state_)
	{
	case STATE::NONE:
		break;
	case STATE::THINK:
	{
		int tmpState = GetRand(1) + 2;
		ChangeState((STATE)tmpState, character);
	}
	break;
	case STATE::IDLE:
		character->animationController_->Play((int)ANIM_TYPE::IDLE);
		step_ = (float)(2000 + GetRand(3000)) / 1000.0f;
		break;
	case STATE::MOVE:
	{
		character->animationController_->Play((int)ANIM_TYPE::WALK);
		step_ = (float)(3000 + GetRand(3000)) / 1000.0f;

		moveDir_ = VECTOR();
		moveDir_.x = ((float)(GetRand(2000)) / 1000.0f) - 1.0f;
		moveDir_.y = 0.0f;
		moveDir_.z = ((float)(GetRand(2000)) / 1000.0f) - 1.0f;
		moveDir_ = VNorm(moveDir_);
	}
	break;
	case STATE::ACTION:
		break;
	case STATE::END:
		break;
	default:
		break;
	}
}

void BossAIComponent::UpdateThink(CharactorBase* character)
{
}

void BossAIComponent::UpdateIdle(CharactorBase* character)
{
	step_ -= SceneManager::GetInstance().GetDeltaTime();
	if (step_ < 0.0f)
	{
		ChangeState(STATE::THINK, character);
	}
}

void BossAIComponent::UpdateMove(CharactorBase* character)
{
	character->movePow_ = VScale(moveDir_, 2.0f);

	auto testPos = VAdd(character->transform_.pos, VScale(character->movePow_, 5.0f));
	VECTOR dirGravity = AsoUtility::DIR_D;
	VECTOR dirUpGravity = AsoUtility::DIR_U;
	float gravityPow = Planet::DEFAULT_GRAVITY_POW;
	float checkPow = 10.0f;

	auto gravHitPosUp = VAdd(testPos, VScale(dirUpGravity, gravityPow));
	gravHitPosUp = VAdd(gravHitPosUp, VScale(dirUpGravity, checkPow * 2.0f));
	auto gravHitPosDown = VAdd(testPos, VScale(dirGravity, checkPow * 10.0f));

	for (const auto c : character->colliders_)
	{
		auto hit = MV1CollCheck_Line(c.lock()->modelId_, -1, gravHitPosUp, gravHitPosDown);
		if (hit.HitFlag == 0)
		{
			moveDir_ = VScale(moveDir_, -1.0f);
			character->movePow_ = VScale(moveDir_, 4.0f);
		}
	}

	character->transform_.quaRot = Quaternion::Slerp(character->transform_.quaRot, Quaternion::LookRotation(moveDir_), 0.1f);

	step_ -= SceneManager::GetInstance().GetDeltaTime();
	if (step_ < 0.0f)
	{
		ChangeState(STATE::THINK, character);
	}
}

void BossAIComponent::UpdateAction(CharactorBase* character)
{
	step_ -= SceneManager::GetInstance().GetDeltaTime();
	if (step_ < 0.0f)
	{
		ChangeState(STATE::THINK, character);
	}
}
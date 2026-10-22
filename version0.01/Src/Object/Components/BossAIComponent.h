#pragma once
#include "../Component.h"
#include "../../Utility/AsoUtility.h"

// ëOï˚êÈåæ
class CharactorBase;

class BossAIComponent : public Component
{
public:
	enum class STATE { NONE, THINK, IDLE, MOVE, ACTION, END };

	BossAIComponent();
	~BossAIComponent() override = default;

	void Update() override;

private:
	STATE state_;
	float step_;
	VECTOR moveDir_;

	void ChangeState(STATE state, CharactorBase* character);
	void UpdateThink(CharactorBase* character);
	void UpdateIdle(CharactorBase* character);
	void UpdateMove(CharactorBase* character);
	void UpdateAction(CharactorBase* character);
};
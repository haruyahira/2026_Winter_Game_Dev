#pragma once
<<<<<<< HEAD
#include "WanderAIComponent.h"

class BossAIComponent : public WanderAIComponent
{
public:
	BossAIComponent(void);
	~BossAIComponent(void) override = default;
};
=======
#include "../Component.h"
#include "../../Utility/AsoUtility.h"

// ‘O•ûéŒ¾
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
>>>>>>> f9ee5eae87468ea5e2cf6eeaa42714623e2e46bd

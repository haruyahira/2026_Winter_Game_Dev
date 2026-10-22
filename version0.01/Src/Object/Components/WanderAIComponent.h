#pragma once
#include <DxLib.h>
#include "../Component.h"

class WanderAIComponent : public Component
{
public:
	enum class STATE
	{
		NONE,
		THINK,
		IDLE,
		MOVE,
		ACTION,
		END
	};

	WanderAIComponent(int animIdle, int animMove, float moveSpeed, int thinkRandRange = 1);
	~WanderAIComponent(void) override = default;

	void Update(void) override;

	STATE GetState(void) const { return state_; }

protected:
	STATE state_;
	float step_;
	VECTOR moveDir_;

	int animIdle_;
	int animMove_;
	float moveSpeed_;
	int thinkRandRange_;

	virtual void ChangeState(STATE state);
	virtual void UpdateThink(void);
	virtual void UpdateIdle(void);
	virtual void UpdateMove(void);
	virtual void UpdateAction(void);
};

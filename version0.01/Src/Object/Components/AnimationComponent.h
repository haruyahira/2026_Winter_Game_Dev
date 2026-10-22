#pragma once
#include <memory>
#include <string>
#include "../Component.h"
#include "../Common/AnimationController.h"

class AnimationComponent : public Component
{
public:
	AnimationComponent(void);
	~AnimationComponent(void) override;

	void Setup(int modelId);
	void Update(void) override;

	void Add(int type, const std::string& path, float speed);
	void AddInFBX(int type, float speed);
	void Play(int type, bool isLoop = true, float startStep = 0.0f, float endStep = -1.0f, bool isStop = false, bool isForce = false);
	void SetEndLoop(float startStep, float endStep, float speed);
	int GetPlayType(void) const;
	bool IsEnd(void) const;

	AnimationController* GetController(void) const { return controller_.get(); }

private:
	std::unique_ptr<AnimationController> controller_;
};

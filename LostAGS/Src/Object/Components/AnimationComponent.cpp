#include "AnimationComponent.h"

AnimationComponent::AnimationComponent(void)
	: controller_(nullptr)
{
}

AnimationComponent::~AnimationComponent(void)
{
}

void AnimationComponent::Setup(int modelId)
{
	controller_ = std::make_unique<AnimationController>(modelId);
}

void AnimationComponent::Update(void)
{
	if (controller_)
	{
		controller_->Update();
	}
}

void AnimationComponent::Add(int type, const std::string& path, float speed)
{
	if (controller_) controller_->Add(type, path, speed);
}

void AnimationComponent::AddInFBX(int type, float speed)
{
	if (controller_) controller_->AddInFBX(type, speed);
}

void AnimationComponent::Play(int type, bool isLoop, float startStep, float endStep, bool isStop, bool isForce)
{
	if (controller_) controller_->Play(type, isLoop, startStep, endStep, isStop, isForce);
}

void AnimationComponent::SetEndLoop(float startStep, float endStep, float speed)
{
	if (controller_) controller_->SetEndLoop(startStep, endStep, speed);
}

int AnimationComponent::GetPlayType(void) const
{
	return controller_ ? controller_->GetPlayType() : -1;
}

bool AnimationComponent::IsEnd(void) const
{
	return controller_ ? controller_->IsEnd() : true;
}

#include "FpsController.h"
#include <DxLib.h>

FpsController::FpsController()
	: startTime_(0), count_(0), fps_(0.0f)
{
}

void FpsController::Update()
{
	if (count_ == 0)
	{
		startTime_ = GetNowCount();
	}

	if (count_ == UPDATE_FRAME)
	{
		int t = GetNowCount();
		fps_ = 1000.0f / ((t - startTime_) / (float)UPDATE_FRAME);
		count_ = 0;
		startTime_ = t;
	}
	count_++;
}

void FpsController::Draw()
{
	// ç∂è„Ç…îíï∂éöÇ≈FPSÇï`âÊ
	DrawFormatString(10, 10, GetColor(255, 255, 255), "FPS: %.1f", fps_);
}
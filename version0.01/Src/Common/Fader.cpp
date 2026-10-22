// Src/Common/Fader.cpp
#include <DxLib.h>
#include "../Application.h"
#include "../Manager/SceneManager.h"
#include "Fader.h"

Fader::STATE Fader::GetState(void) const
{
	return state_;
}

bool Fader::IsEnd(void) const
{
	return isEnd_;
}

void Fader::SetFade(STATE state)
{
	state_ = state;
	if (state_ != STATE::NONE)
	{
		isPreEnd_ = false;
		isEnd_ = false;
	}
}

void Fader::Init(void)
{
	state_ = STATE::NONE;
	alpha_ = 0;
	isPreEnd_ = true;
	isEnd_ = true;
}

void Fader::Update(void)
{
	if (isEnd_)
	{
		return;
	}

	float dtScale = SceneManager::GetInstance().GetDeltaTime() * 60.0f;

	switch (state_)
	{
	case STATE::NONE:
		return;

	case STATE::FADE_OUT:
		alpha_ += SPEED_ALPHA * dtScale;
		if (alpha_ > 255)
		{
			alpha_ = 255;
			if (isPreEnd_)
			{
				isEnd_ = true;
			}
			isPreEnd_ = true;
		}
		break;

	case STATE::FADE_IN:
		alpha_ -= SPEED_ALPHA * dtScale;
		if (alpha_ < 0)
		{
			alpha_ = 0;
			if (isPreEnd_)
			{
				isEnd_ = true;
			}
			isPreEnd_ = true;
		}
		break;

	default:
		return;
	}
}

void Fader::Draw(void)
{
	switch (state_)
	{
	case STATE::NONE:
		return;
	case STATE::FADE_OUT:
	case STATE::FADE_IN:
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)alpha_);
		// ★ フルスクリーンの実解像度に合わせて画面全体を暗転させる
		DrawBox(
			0, 0,
			Application::GetScreenWidth(),
			Application::GetScreenHeight(),
			0x000000, true);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
		break;
	}
}
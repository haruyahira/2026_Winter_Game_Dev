#include <DxLib.h>
#include "../../../Application.h"
#include "ChantSystem.h"



ChantSystem::ChantSystem(void)
	: isChanting_(false)
	, isFinished_(false)
	, typedText_("")
{
}

ChantSystem::~ChantSystem(void)
{
}

void ChantSystem::Start(void)
{
	isChanting_ = true;
	isFinished_ = false;
	typedText_ = "";

	// DxLibの入力バッファをクリア（直前の無駄な入力を消す）
	ClearInputCharBuf();
}

void ChantSystem::Cancel(void)
{
	isChanting_ = false;
	isFinished_ = false;
}

void ChantSystem::Update(void)
{
	if (!isChanting_) return;

	// DxLibの機能：入力された文字を1文字ずつ取り出す
	char c;
	while ((c = GetInputChar(TRUE)) != 0)
	{
		// Backspace（文字を消す）
		if (c == '\b')
		{
			if (!typedText_.empty())
			{
				typedText_.pop_back();
			}
		}
		// Enterキー（詠唱完了！）
		else if (c == '\r' || c == '\n')
		{
			isFinished_ = true;
			break;
		}
		// 通常の文字（アルファベットやスペースなど）
		else if (c >= 32 && c <= 126)
		{
			typedText_ += c;
		}
	}
}

void ChantSystem::Draw(void)
{
	if (!isChanting_) return;

	// ① まず表示したい文字列を作る
	std::string text1 = "詠唱中... [ " + typedText_ + " ]";
	std::string text2 = "※Enterで発動！";

	// ② DXライブラリの機能で、その文字列が画面上で何ピクセルの幅になるか計算する
	int width1 = GetDrawStringWidth(text1.c_str(), text1.length());
	int width2 = GetDrawStringWidth(text2.c_str(), text2.length());

	// ③ 画面の中央(X/2)から、文字幅の半分(width/2)を引いた座標をスタート地点にする
	int drawX1 = (Application::SCREEN_SIZE_X / 2) - (width1 / 2);
	int drawX2 = (Application::SCREEN_SIZE_X / 2) - (width2 / 2);

	// ④ 計算したX座標を使って描画する（Formatは済んでいるので DrawString を使います）
	DrawString(drawX1, 550, text1.c_str(), GetColor(0, 255, 255));
	DrawString(drawX2, 580, text2.c_str(), GetColor(255, 255, 255));
}
#include <DxLib.h>
#include <vector>
#include <cctype>
#include "../../../Application.h"
#include "../../../Manager/InputManager.h"
#include "../../../Utility/AsoUtility.h"
#include "ChantSystem.h"

ChantSystem::ChantSystem(void)
	: isChanting_(false)
	, isFinished_(false)
	, typedText_("")
	, hintText_("【Enter/左ｸﾘｯｸ】発動 / 【Tab】取消\n(例: fire ball/water ball/thunder ball/wood ball)")
	, fontMain_(-1)
	, fontSub_(-1)
	, lastScreenHeight_(0)
{
}

ChantSystem::~ChantSystem(void)
{
	if (fontMain_ != -1) DeleteFontToHandle(fontMain_);
	if (fontSub_ != -1) DeleteFontToHandle(fontSub_);
}

void ChantSystem::UpdateFonts(int screenH)
{
	if (screenH == lastScreenHeight_ && fontMain_ != -1 && fontSub_ != -1) return;

	if (fontMain_ != -1) DeleteFontToHandle(fontMain_);
	if (fontSub_ != -1) DeleteFontToHandle(fontSub_);

	lastScreenHeight_ = screenH;

	int mainFontSize = static_cast<int>(screenH * 0.040f);
	if (mainFontSize < 22) mainFontSize = 22;

	int subFontSize = static_cast<int>(screenH * 0.022f);
	if (subFontSize < 14) subFontSize = 14;

	fontMain_ = CreateFontToHandle(NULL, mainFontSize, 4, DX_FONTTYPE_ANTIALIASING_EDGE_8X8);
	fontSub_ = CreateFontToHandle(NULL, subFontSize, 2, DX_FONTTYPE_ANTIALIASING_EDGE_4X4);
}

void ChantSystem::Start(void)
{
	isChanting_ = true;
	isFinished_ = false;
	typedText_ = "";
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

	// ★ 左クリックでも発動！
	if (InputManager::GetInstance().IsTrgMouseLeft())
	{
		isFinished_ = true;
	}

	char c;
	while ((c = GetInputChar(TRUE)) != 0)
	{
		if (c == '\b')
		{
			if (!typedText_.empty()) typedText_.pop_back();
		}
		else if (c == '\r' || c == '\n')
		{
			isFinished_ = true;
			break;
		}
		else if (c >= 32 && c <= 126)
		{
			typedText_ += c;
		}
	}
}

// ★ 追加：スペースを消して小文字にした「判定用文字列」を返す
std::string ChantSystem::GetNormalizedSpell(void) const
{
	std::string normalized = "";
	for (char c : typedText_)
	{
		if (c != ' ')
		{
			normalized += static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
		}
	}
	return normalized;
}

void ChantSystem::Draw(void)
{
	if (!isChanting_) return;

	int screenW = Application::GetScreenWidth();
	int screenH = Application::GetScreenHeight();

	UpdateFonts(screenH);

	int marginX = static_cast<int>(screenW * 0.03f);
	int boxLeft = marginX;
	int boxRight = screenW - marginX;
	int boxBottom = screenH - static_cast<int>(screenH * 0.04f);

	// ウィンドウの高さを少しだけ広げる
	int boxHeight = static_cast<int>(screenH * 0.18f);
	int boxTop = boxBottom - boxHeight;

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 200);
	DrawBox(boxLeft, boxTop, boxRight, boxBottom, GetColor(10, 15, 30), TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	DrawBox(boxLeft, boxTop, boxRight, boxBottom, GetColor(0, 220, 255), FALSE);
	DrawBox(boxLeft + 2, boxTop + 2, boxRight - 2, boxBottom - 2, GetColor(0, 120, 180), FALSE);

	int innerMargin = static_cast<int>(screenW * 0.02f);
	int inputLeft = boxLeft + innerMargin;
	int inputRight = boxRight - innerMargin;

	int inputTop = boxTop + static_cast<int>(boxHeight * 0.18f);
	int inputBottom = boxTop + static_cast<int>(boxHeight * 0.58f);

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 220);
	DrawBox(inputLeft, inputTop, inputRight, inputBottom, GetColor(0, 0, 0), TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	DrawBox(inputLeft, inputTop, inputRight, inputBottom, GetColor(80, 200, 255), FALSE);

	bool showCursor = ((GetNowCount() / 400) % 2) == 0;
	std::string text1 = "SPELL CHANT :  " + typedText_ + (showCursor ? "_" : " ");

	int mainFontH = static_cast<int>(screenH * 0.040f);
	int drawY1 = inputTop + ((inputBottom - inputTop) - mainFontH) / 2;
	int drawX1 = inputLeft + static_cast<int>(screenW * 0.015f);

	DrawStringToHandle(drawX1, drawY1, text1.c_str(), GetColor(0, 255, 255), fontMain_, GetColor(0, 40, 60));

	// ★ 改行対応のヒントテキストを中央下に描画
	std::string hintStr = hintText_;
	std::vector<std::string> hintLines = AsoUtility::Split(hintStr, '\n');

	int drawY2 = inputBottom + static_cast<int>(boxHeight * 0.05f);
	int subFontH = GetFontSizeToHandle(fontSub_);

	for (size_t i = 0; i < hintLines.size(); ++i)
	{
		int width2 = GetDrawStringWidthToHandle(hintLines[i].c_str(), static_cast<int>(hintLines[i].length()), fontSub_);
		int drawX2 = (screenW / 2) - (width2 / 2);
		DrawStringToHandle(drawX2, drawY2 + i * (subFontH + 4), hintLines[i].c_str(), GetColor(220, 220, 220), fontSub_, GetColor(0, 0, 0));
	}
}
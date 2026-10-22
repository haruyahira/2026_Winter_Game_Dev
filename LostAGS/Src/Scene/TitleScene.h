// Src/Scene/TitleScene.h
#pragma once
#include <memory>
#include <string>
#include "SceneBase.h"
#include "../Object/Common/Transform.h"

class SkyDome;
class ChantSystem;

class TitleScene : public SceneBase
{
public:
	TitleScene(void);
	~TitleScene(void) override;

	void Init(void) override;
	void Update(void) override;
	void Draw(void) override;

private:
	Transform planetTransform_;
	std::unique_ptr<SkyDome> skyDome_;
	std::unique_ptr<ChantSystem> chantSystem_;

	float animTime_;
	bool isError_;
	float errorTimer_;

	int fontTitle_;
	int fontSub_;
	int fontGuide_;
	int lastScreenH_;

	void UpdateFonts(int screenH);
	void DrawMagicCircle(int cx, int cy, float radius, float angle, unsigned int color);
};
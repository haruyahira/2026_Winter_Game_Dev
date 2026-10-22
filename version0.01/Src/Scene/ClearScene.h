// Src/Scene/ClearScene.h
#pragma once
#include <memory>
#include <string>
#include "SceneBase.h"
#include "../Object/Common/Transform.h"

class SkyDome;
class AnimationController;
class ChantSystem;

class ClearScene : public SceneBase
{
public:
	ClearScene(void);
	~ClearScene(void) override;

	void Init(void) override;
	void Update(void) override;
	void Draw(void) override;

private:
	Transform planetTransform_;
	Transform playerTransform_;
	std::unique_ptr<AnimationController> playerAnim_;
	std::unique_ptr<SkyDome> skyDome_;
	std::unique_ptr<ChantSystem> chantSystem_;

	float animTime_;
	bool isError_;
	float errorTimer_;

	int fontClear_;
	int fontSub_;
	int fontGuide_;
	int lastScreenH_;

	void UpdateFonts(int screenH);
	void DrawVictoryCircle(int cx, int cy, float radius, float angle, unsigned int color);
};
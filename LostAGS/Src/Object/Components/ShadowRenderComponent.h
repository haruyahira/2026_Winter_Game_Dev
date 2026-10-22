#pragma once
#include <DxLib.h>
#include "../Component.h"

class ShadowRenderComponent : public Component
{
public:
	static constexpr float DEFAULT_SHADOW_HEIGHT = 300.0f;
	static constexpr float DEFAULT_SHADOW_SIZE = 30.0f;

	ShadowRenderComponent(void);
	~ShadowRenderComponent(void) override = default;

	void Setup(float shadowSize = DEFAULT_SHADOW_SIZE, float shadowHeight = DEFAULT_SHADOW_HEIGHT);
	void Draw(void) override;

private:
	int imgShadow_;
	float shadowSize_;
	float shadowHeight_;
};

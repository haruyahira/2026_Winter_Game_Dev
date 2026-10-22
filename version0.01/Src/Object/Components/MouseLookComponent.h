// MouseLookComponent.h
#pragma once
#include <DxLib.h>
#include "../Component.h"
#include "../../Common/Quaternion.h"

class MouseLookComponent : public Component
{
public:
	MouseLookComponent(void);
	~MouseLookComponent(void) override = default;

	void Update(void) override;

	const Quaternion& GetPlayerRotation(void) const { return playerRotY_; }
	void SetPlayerRotation(const Quaternion& rot) { playerRotY_ = rot; }

private:
	Quaternion playerRotY_;
};
#pragma once
#include <memory>
#include <map>
#include <functional>
#include <DxLib.h>
#include "../Component.h"
#include "../../Common/Quaternion.h"
#include "../Charactor/Player/Player.h"

class ChantSystem;
class ActorBase;

class PlayerControlComponent : public Component
{
public:
	using STATE = Player::STATE;
	using ANIM_TYPE = Player::ANIM_TYPE;

	PlayerControlComponent(void);
	~PlayerControlComponent(void) override;

	void Init(void) override;
	void Update(void) override;
	void Draw(void) override;

	void OnLanding(void);

	void SetSpawnFunc(std::function<void(std::unique_ptr<ActorBase>)> func)
	{
		spawnActorFunc_ = func;
	}

private:
	std::unique_ptr<ChantSystem> chantSystem_;

	STATE state_;
	std::map<STATE, std::function<void(void)>> stateChanges_;
	std::function<void(void)> stateUpdate_;

	std::function<void(std::unique_ptr<ActorBase>)> spawnActorFunc_;

	Quaternion playerRotY_;
	Quaternion goalQuaRot_;
	float stepRotTime_;
	float stepJump_;

	void ChangeState(STATE state);
	void ChangeStateNone(void);
	void ChangeStatePlay(void);
	void ChangeStateChant(void);

	void UpdateNone(void);
	void UpdatePlay(void);
	void UpdateChant(void);

	void ProcessMove(void);
	void ProcessJump(void);
	void ProcessDefense(void);
	void ProcessAttack(void);
	void ProcessMagic(const std::string& spellName);

	void SetGoalRotate(double rotRad);
	void Rotate(void);
	bool IsEndLanding(void);
};

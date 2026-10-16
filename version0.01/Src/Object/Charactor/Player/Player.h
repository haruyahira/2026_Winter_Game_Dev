#pragma once
#include <map>
#include <functional>
#include "../CharactorBase.h"

class Player : public CharactorBase
{
public:
	static constexpr float SPEED_MOVE = 5.0f;
	static constexpr float SPEED_RUN = 10.0f;
	static constexpr float TIME_ROT = 1.0f;
	static constexpr float POW_JUMP = 35.0f;
	static constexpr float TIME_JUMP_IN = 0.5f;

	enum class STATE { NONE, PLAY, WARP_RESERVE, WARP_MOVE, DEAD, VICTORY, END };
	enum class ANIM_TYPE { IDLE, RUN, FAST_RUN, JUMP, WARP_PAUSE, FLY, FALLING, VICTORY };

	Player(void);
	~Player(void);

	void Update(void) override;
	void Draw(void) override;

protected:
	// CharactorBaseから委譲された固有初期化と着地処理
	void InitDetail(void) override;
	void OnLanding(void) override;

private:
	// 状態管理
	STATE state_;
	std::map<STATE, std::function<void(void)>> stateChanges_;
	std::function<void(void)> stateUpdate_;

	// 回転
	Quaternion playerRotY_;
	Quaternion goalQuaRot_;
	float stepRotTime_;

	// ジャンプ受付
	float stepJump_;

	void InitAnimation(void);

	void ChangeState(STATE state);
	void ChangeStateNone(void);
	void ChangeStatePlay(void);
	void UpdateNone(void);
	void UpdatePlay(void);

	// 操作
	void ProcessMove(void);
	void ProcessJump(void);
	void SetGoalRotate(double rotRad);
	void Rotate(void);

	bool IsEndLanding(void);
};
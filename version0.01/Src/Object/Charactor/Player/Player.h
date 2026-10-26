#pragma once
#include <memory>
#include <functional>
#include "../../../Utility/AsoUtility.h"
#include "../../Magic/MagicBase.h"
#include "../CharactorBase.h"

class PlayerControlComponent;

class Player : public CharactorBase
{
public:

	// スピード
	static constexpr float SPEED_MOVE = 5.0f;
	static constexpr float SPEED_RUN = 10.0f;

	// 回転完了までの時間
	static constexpr float TIME_ROT = 1.0f;

	// ジャンプ力
	static constexpr float POW_JUMP = 35.0f;

	// ジャンプ受付時間
	static constexpr float TIME_JUMP_IN = 0.5f;

	// モデルの基本設定
	static constexpr VECTOR INIT_POSITION = { 0.0f, 26.0f, 0.0f };  // プレイヤー初期座標
	static constexpr VECTOR INIT_AXIS = { 0.0f, 180.0f, 0.0f };  // プレイヤー初期軸
	static constexpr float INIT_SCALE = 1.0f;  // プレイヤー初期スケール

	// 当たり判定（カプセル）のサイズ設定
	static constexpr VECTOR CAPSULE_TOP = { 0.0f, 110.0f, 0.0f };  // カプセル上端座標
	static constexpr VECTOR CAPSULE_BOTTOM = { 0.0f, 30.0f, 0.0f };  // カプセル下端座標
	static constexpr float CAPSULE_RADIUS = 20.0f;  // カプセルの半径

	// ジャンプ＆着地アニメーションのフレーム指定
	static constexpr float ANIM_JUMP_START = 13.0f;  // ジャンプ開始フレーム
	static constexpr float ANIM_JUMP_LOOP_START = 23.0f;  // 滞空ループの開始フレーム
	static constexpr float ANIM_JUMP_LOOP_END = 25.0f;  // 滞空ループの終了フレーム
	static constexpr float ANIM_JUMP_BLEND = 5.0f;  // 滞空ループのブレンド時間
	static constexpr float ANIM_LANDING_START = 29.0f;  // 着地モーション開始フレーム
	static constexpr float ANIM_LANDING_END = 45.0f;  // 着地モーション終了フレーム

	// プレイヤーの状態
	enum class STATE 
	{ 
		NONE,
		PLAY,
		CHANT,  // 詠唱
		WARP_RESERVE,
		WARP_MOVE,
		DEAD,
		VICTORY,
		END
	};

	// アニメーション種別
	enum class ANIM_TYPE 
	{ 
		IDLE, 
		RUN, 
		FAST_RUN, 
		JUMP, 
		WARP_PAUSE, 
		FLY, 
		FALLING, 
		VICTORY 
	};

	Player(void);
	~Player(void) override;

	void SetSpawnFunc(std::function<void(std::unique_ptr<ActorBase>)> func);

protected:

	void InitDetail(void) override;
	void OnLanding(void) override;

private:

	std::shared_ptr<PlayerControlComponent> controlComp_;
	std::function<void(std::unique_ptr<ActorBase>)> spawnActorFunc_;

	void InitAnimation(void);
};

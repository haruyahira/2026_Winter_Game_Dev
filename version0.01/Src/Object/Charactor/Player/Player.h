// Src/Object/Charactor/Player/Player.h
#pragma once
#include <memory>
#include <functional>
#include <string>
#include "../../../Utility/AsoUtility.h"
#include "../../Magic/MagicBase.h"
#include "../CharactorBase.h"

class PlayerControlComponent;

class Player : public CharactorBase
{
public:
	struct ShaderParam
	{
		float progress;     // 進行度
		float edgeWidth;    // 境界線の幅
		float dummy[2];     // ★超重要：メモリの隙間を埋めるためのダミー
		float edgeColor[4]; // 境界線の色
	};

	// 頂点シェーダーへ送る行列データ
	struct VS_ShaderParam
	{
		MATRIX matView;
		MATRIX matProj;
	};

	// スピード
	static constexpr float SPEED_MOVE = 5.0f;
	static constexpr float SPEED_RUN = 50.0f;
	static constexpr float TIME_ROT = 1.0f;
	static constexpr float POW_JUMP = 35.0f;
	static constexpr float TIME_JUMP_IN = 0.5f;

	// モデルの基本設定
	static constexpr VECTOR INIT_POSITION = { 0.0f, 100.0f, 0.0f };
	static constexpr VECTOR INIT_AXIS = { 0.0f, 180.0f, 0.0f };
	static constexpr float INIT_SCALE = 1.5f;

	// 当たり判定
	static constexpr VECTOR CAPSULE_TOP = { 0.0f, 110.0f, 0.0f };
	static constexpr VECTOR CAPSULE_BOTTOM = { 0.0f, 30.0f, 0.0f };
	static constexpr float CAPSULE_RADIUS = 20.0f;

	// アニメーション設定
	static constexpr float ANIM_JUMP_START = 13.0f;
	static constexpr float ANIM_JUMP_LOOP_START = 23.0f;
	static constexpr float ANIM_JUMP_LOOP_END = 25.0f;
	static constexpr float ANIM_JUMP_BLEND = 5.0f;
	static constexpr float ANIM_LANDING_START = 29.0f;
	static constexpr float ANIM_LANDING_END = 45.0f;

	enum class STATE
	{
		NONE, PLAY, CHANT, WARP_RESERVE, WARP_MOVE, DEAD, VICTORY, END
	};

	enum class ANIM_TYPE
	{
		IDLE, RUN, FAST_RUN, JUMP, WARP_PAUSE, FLY, FALLING, VICTORY
	};

	Player(void);
	~Player(void) override;

	void StartCasting(const std::string& spellName);
	void Update(void) override;
	void Draw(void) override;
	void SetSpawnFunc(std::function<void(std::unique_ptr<ActorBase>)> func);

protected:
	void InitDetail(void) override;
	void OnLanding(void) override;

private:
	std::shared_ptr<PlayerControlComponent> controlComp_;
	std::function<void(std::unique_ptr<ActorBase>)> spawnActorFunc_;

	void InitAnimation(void);

	// 魔法陣用のリソース
	int magicBaseImg_;
	int magicMaskImg_;

	int vertexShader_;     // ★頂点シェーダー
	int vsConstBuffer_;    // ★頂点シェーダー用バッファ
	int pixelShader_;
	int constBuffer_;

	// 詠唱管理用
	bool isCasting_;
	float currentCastTime_;
	std::string castingSpell_;
};
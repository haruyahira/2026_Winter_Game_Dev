#pragma once
#include "../Common/Quaternion.h"
#include "ActorBase.h"
#include "Stage.h"
class ModelMaterial;
class ModelRenderer;

class Planet : public ActorBase
{

public:

	// 重力の強さ
	static constexpr float DEFAULT_GRAVITY_POW = 25.0f;

	// 重力がかかる範囲
	static constexpr float DEFAULT_GRAVITY_RADIUS = 5000.0f;

	// ゲームオーバー範囲
	static constexpr float DEFAULT_DEAD_LENGTH = 1000.0f;

	// 重力種別
	enum class TYPE
	{
		GROUND,
		SPHERE,
		TRANS_ROT,
		TRANS_CAMERA_FIXED,
		ROAD,
	};

	// コンストラクタ
	Planet(const Stage::NAME& name, const TYPE& type, const Transform& transform);

	// デストラクタ
	~Planet(void);

	void Init(void) override;
	void Update(void) override;
	void Draw(void) override;
	
private:

	// 惑星種別
	TYPE type_;

	// 惑星名
	Stage::NAME name_;

	// モデル描画用用
	std::unique_ptr<ModelMaterial> material_;
	std::unique_ptr<ModelRenderer> renderer_;

};

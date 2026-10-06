#pragma once
#include <vector>
#include <map>
#include <functional>
#include <DxLib.h>
#include "Common/Transform.h"
class AnimationController;
class Collider;
class Capsule;
class ModelMaterial;
class ModelRenderer;

class Destroyer
{

public:

	// 状態
	enum class STATE
	{
		NONE,
		THINK,
		IDLE,
		MOVE,
		ACTION,
		END
	};

	// アニメーション種別
	enum class ANIM_TYPE
	{
		IDLE = 3,
		RUN = 10,
	};

	// コンストラクタ
	Destroyer(void);

	// デストラクタ
	~Destroyer(void);

	void Init(void);
	void Update(void);
	void Draw(void);

	bool isDestroy();

	// 衝突判定に用いられるコライダ制御
	void AddCollider(std::weak_ptr<Collider> collider);

	void ClearCollider(void);

protected:

	// アニメーション
	std::unique_ptr<AnimationController> animationController_;

	// 状態管理
	STATE state_;

	// モデル描画用
	std::unique_ptr<ModelMaterial> material_;
	std::unique_ptr<ModelRenderer> renderer_;

	// モデル制御の基本情報
	Transform transform_;

	// 丸影
	int imageShadow_;

	// トゥーンマップ
	int imageToonMap_;

	// 衝突判定に用いられるコライダ
	std::vector<std::weak_ptr<Collider>> colliders_;
	std::unique_ptr<Capsule> capsule_;

	float speed_;
	VECTOR moveDir_;
	VECTOR movePow_;
	VECTOR movedPos_;

	// ジャンプ量
	VECTOR jumpPow_;

	// 更新ステップ
	float step_;

	// 状態遷移
	void ChangeState(STATE state);

	// 更新系
	void UpdateThink(void);
	void UpdateIdle(void);
	void UpdateMove();
	void UpdateAction(void);

	// 描画系
	void DrawShadow(void);
	
	// 衝突判定
	void Collision(void);
	void CollisionCapsule();

	// 移動量の計算
	void CalcGravityPow(void);

private:

	// 初期座標
	static constexpr VECTOR DEFAULT_POS = { 150.0f, -40.0f, 200.0f };

	// 初期角度
	static constexpr float DEFAULT_ROT_Y_DEG = 180.0f;
};

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

class Centaur
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
		IDLE = 13,
		WALK = 14,
	};

	// コンストラクタ
	Centaur(void);

	// デストラクタ
	~Centaur(void);

	void Init(void);
	void Update(void);
	void Draw(void);

	// 衝突判定に用いられるコライダ制御
	void AddCollider(std::weak_ptr<Collider> collider);
	void ClearCollider(void);

protected:

	// アニメーション
	std::unique_ptr<AnimationController> animationController_;

	// 状態管理
	STATE state_;

	// モデル描画用用
	std::unique_ptr<ModelMaterial> material_;
	std::unique_ptr<ModelRenderer> renderer_;

	// モデル制御の基本情報
	Transform transform_;

	// 丸影
	int imgShadow_;

	// トゥーンマップ
	int imgToonMap_;

	// 衝突判定に用いられるコライダ
	std::vector<std::weak_ptr<Collider>> colliders_;
	std::unique_ptr<Capsule> capsule_;

	// 移動スピード
	float speed_;

	// 移動方向
	VECTOR moveDir_;

	// 移動量
	VECTOR movePow_;

	// 移動後の座標
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
	void UpdateMove(void);
	void UpdateAction(void);

	// 描画系
	void DrawShadow(void);

	// 衝突判定
	void Collision(void);
	void CollisionGravity(void);
	void CollisionCapsule(void);

	// 移動量の計算
	void CalcGravityPow(void);

};


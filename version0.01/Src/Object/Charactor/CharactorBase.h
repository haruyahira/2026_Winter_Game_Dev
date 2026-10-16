#pragma once
#include <memory>
#include <vector>
#include <string> // 追加
#include <DxLib.h>
#include "../ActorBase.h"

class AnimationController;
class Collider;
class Capsule;
class ModelMaterial;
class ModelRenderer;

class CharactorBase : public ActorBase
{
public:
	CharactorBase(void);
	virtual ~CharactorBase(void);

	// 初期化の大枠を固定（オーバーライド禁止）
	virtual void Init(void) override final;
	virtual void Update(void) override = 0;
	virtual void Draw(void) override = 0;

	// 衝突判定に用いられるコライダ制御
	void AddCollider(std::weak_ptr<Collider> collider);
	void ClearCollider(void);
	const Capsule& GetCapsule(void) const;

protected:

	// 子クラス固有の初期化処理（モデル・カプセル・アニメーションの設定など）
	virtual void InitDetail(void) = 0;

	// ★追加: シェーダーファイル名
	std::string vsName_;
	std::string psName_;

	// アニメーションと描画
	std::unique_ptr<AnimationController> animationController_;
	std::unique_ptr<ModelMaterial> material_;
	std::unique_ptr<ModelRenderer> renderer_;

	// トゥーンシェーダ・影
	int imgShadow_;
	int imgToonMap_;

	// 衝突判定
	std::vector<std::weak_ptr<Collider>> colliders_;
	std::unique_ptr<Capsule> capsule_;

	// 物理演算用変数
	float speed_;
	VECTOR moveDir_;
	VECTOR movePow_;
	VECTOR movedPos_;
	VECTOR jumpPow_;
	bool isJump_;

	// 衝突チェック用座標
	VECTOR gravHitPosDown_;
	VECTOR gravHitPosUp_;

	// 共通メソッド (Player/Enemy 共通の物理挙動)
	virtual void DrawShadow(void);
	virtual void CalcGravityPow(void);
	virtual void Collision(void);
	virtual void CollisionCapsule(void);
	virtual void CollisionGravity(void);

	// 着地時に呼ばれるフック関数
	virtual void OnLanding(void) {}
};
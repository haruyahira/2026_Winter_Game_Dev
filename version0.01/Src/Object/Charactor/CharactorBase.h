#pragma once
#include <memory>
#include <vector>
#include <string> 
#include <DxLib.h>
#include "../../Manager/ResourceManager.h"
#include "../ActorBase.h"

class AnimationController;
class Collider;
class Capsule;
class ModelRenderComponent;
class ShadowRenderComponent;
class CapsuleColliderComponent;
class PhysicsComponent;
class AnimationComponent;

class CharactorBase : public ActorBase
{
public:
	CharactorBase(void);
	virtual ~CharactorBase(void);

	// 初期化の大枠を固定（オーバーライド禁止）
	virtual void Init(void) override final;
	virtual void Update(void) override;
	virtual void Draw(void) override;

	// 衝突判定に用いられるコライダ制御（CapsuleColliderComponentへ委譲）
	void AddCollider(std::weak_ptr<Collider> collider);
	void ClearCollider(void);
	const Capsule& GetCapsule(void) const;

protected:

	// 子クラス固有の初期化処理（モデル・カプセル・アニメーション・AI部品の設定など）
	virtual void InitDetail(void) = 0;

	void SetShader(const std::string& vsName, const std::string& psName);
	void LoadAndSetupModel(ResourceManager::SRC srcId, float scl, VECTOR pos, VECTOR rot);
	void SetupCapsule(VECTOR top, VECTOR bottom, float radius);
	void HideModelFrame(int frameIndex);

	// シェーダーファイル名・影サイズ設定
	std::string vsName_;
	std::string psName_;
	float shadowSize_;

	// 共通コンポーネント群へのポインタ
	std::shared_ptr<ModelRenderComponent> modelRenderComp_;
	std::shared_ptr<ShadowRenderComponent> shadowRenderComp_;
	std::shared_ptr<CapsuleColliderComponent> capsuleComp_;
	std::shared_ptr<PhysicsComponent> physicsComp_;
	std::shared_ptr<AnimationComponent> animComp_;

	// 着地時に呼ばれるフック関数
	virtual void OnLanding(void) {}
};

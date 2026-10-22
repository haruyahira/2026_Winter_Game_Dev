#include "CharactorBase.h"
#include "../../Manager/ResourceManager.h"
#include "../../Utility/AsoUtility.h"
#include "../Common/Capsule.h"
#include "../Common/Collider.h"
#include "../Components/ModelRenderComponent.h"
#include "../Components/ShadowRenderComponent.h"
#include "../Components/CapsuleColliderComponent.h"
#include "../Components/PhysicsComponent.h"
#include "../Components/AnimationComponent.h"

CharactorBase::CharactorBase(void)
	: vsName_("ToonModelVS6.cso")
	, psName_("ToonModelPS6.cso")
	, shadowSize_(ShadowRenderComponent::DEFAULT_SHADOW_SIZE)
	, modelRenderComp_(nullptr)
	, shadowRenderComp_(nullptr)
	, capsuleComp_(nullptr)
	, physicsComp_(nullptr)
	, animComp_(nullptr)
{
}

CharactorBase::~CharactorBase(void)
{
}

void CharactorBase::Init(void)
{
	// 1. 描画・影・カプセル部品を先に生成して登録（Draw順：モデル -> 影 -> カプセル -> UI）
	modelRenderComp_ = std::make_shared<ModelRenderComponent>();
	AddComponent(modelRenderComp_);

	shadowRenderComp_ = std::make_shared<ShadowRenderComponent>();
	AddComponent(shadowRenderComp_);

	capsuleComp_ = std::make_shared<CapsuleColliderComponent>();
	AddComponent(capsuleComp_);

	// 2. 物理・アニメーション部品を生成（Update順でAI/操作の後に動かすため登録はInitDetailの後）
	physicsComp_ = std::make_shared<PhysicsComponent>();
	physicsComp_->SetOwner(this);
	physicsComp_->SetOnLanding([this]() { this->OnLanding(); });

	animComp_ = std::make_shared<AnimationComponent>();
	animComp_->SetOwner(this);

	// 3. 子クラス固有の初期化（モデル読み込み、カプセル設定、AI/操作コンポーネントの追加）
	InitDetail();

	// 4. モデル読み込み後にシェーダー描画部品と影部品をセットアップ
	modelRenderComp_->Setup(transform_.modelId, vsName_, psName_);
	shadowRenderComp_->Setup(shadowSize_);

	// 5. 物理・アニメーション部品を登録（Update順：AI/操作 -> 物理 -> アニメーション）
	AddComponent(physicsComp_);
	AddComponent(animComp_);
}

void CharactorBase::Update(void)
{
	UpdateComponents();
}

void CharactorBase::Draw(void)
{
	DrawComponents();
}

void CharactorBase::AddCollider(std::weak_ptr<Collider> collider)
{
	if (capsuleComp_)
	{
		capsuleComp_->AddCollider(collider);
	}
}

void CharactorBase::ClearCollider(void)
{
	if (capsuleComp_)
	{
		capsuleComp_->ClearCollider();
	}
}

const Capsule& CharactorBase::GetCapsule(void) const
{
	return capsuleComp_->GetCapsule();
}

void CharactorBase::SetShader(const std::string& vsName, const std::string& psName)
{
	vsName_ = vsName;
	psName_ = psName;
}

void CharactorBase::LoadAndSetupModel(ResourceManager::SRC srcId, float scl, VECTOR pos, VECTOR rot)
{
	transform_.SetModel(ResourceManager::GetInstance().LoadModelDuplicate(srcId));
	transform_.scl = VScale(AsoUtility::VECTOR_ONE, scl);
	transform_.pos = pos;
	transform_.quaRot = Quaternion();
	transform_.quaRotLocal = Quaternion::Euler({ rot.x, AsoUtility::Deg2RadF(rot.y), rot.z });
	transform_.Update();

	if (animComp_)
	{
		animComp_->Setup(transform_.modelId);
	}
}

void CharactorBase::SetupCapsule(VECTOR top, VECTOR bottom, float radius)
{
	if (capsuleComp_)
	{
		capsuleComp_->Setup(top, bottom, radius);
	}
}

void CharactorBase::HideModelFrame(int frameIndex)
{
	MV1SetFrameVisible(transform_.modelId, frameIndex, false);
}

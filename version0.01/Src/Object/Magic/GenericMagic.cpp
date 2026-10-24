// Src/Object/Magic/GenericMagic.cpp
#include "GenericMagic.h"
#include "../../Manager/ResourceManager.h"
#include "../../Manager/SceneManager.h"
#include "../../Utility/AsoUtility.h"
#include "../Components/ProjectileComponent.h"

GenericMagic::GenericMagic(const MagicData& data)
	: data_(data)
	, projectileComp_(nullptr)
	, isExploding_(false)
	, explodeTimer_(0.0f)
{
	// 設計図通りに基本パラメータをセット
	damage_ = data_.damage;
	speed_ = data_.speed;
	collisionRadius_ = data_.collisionRadius;
}

GenericMagic::~GenericMagic(void)
{
}

void GenericMagic::Init(void)
{
	// 設計図通りにモデルと大きさをセット
	transform_.SetModel(ResourceManager::GetInstance().LoadModelDuplicate(data_.modelId));
	transform_.scl = VScale(AsoUtility::VECTOR_ONE, data_.scale);

	projectileComp_ = std::make_shared<ProjectileComponent>(speed_, data_.lifeTime);
	projectileComp_->SetOnExpire([this]() { this->Destroy(); });
	AddComponent(projectileComp_);
}

void GenericMagic::Update(void)
{
	if (!isAlive_) return;

	if (isExploding_)
	{
		explodeTimer_ += SceneManager::GetInstance().GetDeltaTime();
		if (explodeTimer_ >= EXPLODE_DURATION)
		{
			Destroy();
		}
		return;
	}

	UpdateComponents();
}

void GenericMagic::Draw(void)
{
	if (!isAlive_) return;

	if (isExploding_)
	{
		// 設計図通りの色で爆発エフェクトを描画
		float rate = explodeTimer_ / EXPLODE_DURATION;
		float radius = collisionRadius_ * (1.0f + rate * 2.5f);
		int alpha = static_cast<int>(255.0f * (1.0f - rate));

		SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
		DrawSphere3D(transform_.pos, radius, 12, data_.expColorAura, data_.expColorCore, TRUE);
		DrawSphere3D(transform_.pos, radius * 0.6f, 12, data_.expColorCore, GetColor(255, 255, 255), TRUE);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
		return;
	}

	MV1DrawModel(transform_.modelId);
}

bool GenericMagic::CanHit(void) const
{
	return isAlive_ && !isExploding_;
}

void GenericMagic::OnHit(void)
{
	if (isExploding_) return;

	isExploding_ = true;
	explodeTimer_ = 0.0f;
	if (projectileComp_)
	{
		projectileComp_->SetActive(false);
	}
}

bool GenericMagic::isDestroy(void)
{
	return !isAlive_;
}

void GenericMagic::Fire(const Transform& spawnTransform, VECTOR dir)
{
	transform_.pos = spawnTransform.pos;
	isAlive_ = true;
	isExploding_ = false;
	explodeTimer_ = 0.0f;
	if (projectileComp_)
	{
		projectileComp_->SetActive(true);
		projectileComp_->Fire(dir);
	}
}
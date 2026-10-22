#include "FireBall.h"
#include "../../Manager/ResourceManager.h"
#include "../../Manager/SceneManager.h"
#include "../../Utility/AsoUtility.h"
#include "../Components/ProjectileComponent.h"

FireBall::FireBall(void)
	: projectileComp_(nullptr)
	, isExploding_(false)
	, explodeTimer_(0.0f)
{
	damage_ = DEFAULT_DAMAGE;
	speed_ = DEFAULT_SPEED;
	collisionRadius_ = DEFAULT_COLLISION_RADIUS;
}

FireBall::~FireBall(void)
{
}

void FireBall::Init(void)
{
	transform_.SetModel(ResourceManager::GetInstance().LoadModelDuplicate(
		ResourceManager::SRC::FIREBALL));
	transform_.scl = AsoUtility::VECTOR_ONE;

	projectileComp_ = std::make_shared<ProjectileComponent>(speed_, DEFAULT_LIFE_TIME);
	projectileComp_->SetOnExpire([this]() { this->Destroy(); });
	AddComponent(projectileComp_);
}

void FireBall::Update(void)
{
	if (!isAlive_) return;

	// はじけている最中の演出更新
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

void FireBall::Draw(void)
{
	if (!isAlive_) return;

	if (isExploding_)
	{
		// はじける演出：膨張しながら消えていくオレンジ・黄色の閃光を描画
		float rate = explodeTimer_ / EXPLODE_DURATION;
		float radius = collisionRadius_ * (1.0f + rate * 2.5f);
		int alpha = static_cast<int>(255.0f * (1.0f - rate));

		SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
		DrawSphere3D(transform_.pos, radius, 12, GetColor(255, 100, 0), GetColor(255, 255, 0), TRUE);
		DrawSphere3D(transform_.pos, radius * 0.6f, 12, GetColor(255, 255, 180), GetColor(255, 255, 255), TRUE);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
		return;
	}

	MV1DrawModel(transform_.modelId);
}

bool FireBall::CanHit(void) const
{
	// 生存しており、まだはじけていない時だけ当たり判定を持つ
	return isAlive_ && !isExploding_;
}

void FireBall::OnHit(void)
{
	if (isExploding_) return;

	// 飛翔コンポーネントを止めて、はじける演出を開始する
	isExploding_ = true;
	explodeTimer_ = 0.0f;
	if (projectileComp_)
	{
		projectileComp_->SetActive(false);
	}
}

bool FireBall::isDestroy(void)
{
	return !isAlive_;
}

void FireBall::Fire(const Transform& spawnTransform, VECTOR dir)
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
#include <DxLib.h>
#include "../Utility/AsoUtility.h"
#include "../Manager/ResourceManager.h"
#include "../Manager/Resource.h"
#include "../Manager/SceneManager.h"
#include "../Manager/Camera.h"
#include "../Renderer/ModelMaterial.h"
#include "../Renderer/ModelRenderer.h"
#include "Common/Transform.h"
#include "Skull.h"

Skull::Skull(void)
{
}

Skull::~Skull(void)
{
}

void Skull::Init(void)
{

	transform_.SetModel(
		resMng_.LoadModelDuplicate(ResourceManager::SRC::SKULL));
	transform_.scl = VScale(AsoUtility::VECTOR_ONE, 0.3f);
	transform_.quaRot = Quaternion();
	transform_.pos = { 0.0f, -100.0f, 0.0f };

	// モデル描画用
	material_ = std::make_unique<ModelMaterial>(
		"SkullModelVS4.cso", 2,
		"SkullModelPS4.cso", 4
		);

	// 拡散光
	material_->AddConstBufPS({ 1.0f, 1.0f, 1.0f, 1.0f });

	// 環境光
	material_->AddConstBufPS({ 0.2f, 0.2f, 0.2f, 1.0f });

	// 頂点①：カメラ座標
	auto& camera = SceneManager::GetInstance().GetCamera();
	auto cPos = camera.GetPos();
	material_->AddConstBufVS({ cPos.x, cPos.y, cPos.z, 0.0f });

	// 頂点②：フォグ開始・終了距離
	float fogStart;
	float fogEnd;
	GetFogStartEnd(&fogStart, &fogEnd);
	material_->AddConstBufVS({ fogStart, fogEnd, 0.0f, 0.0f });

	// ライトの方向
	VECTOR worldLightDirection = GetLightDirection();
	material_->AddConstBufPS(
		{ worldLightDirection.x, worldLightDirection.y, worldLightDirection.z, 0.0f });

	material_->AddConstBufPS({ 0.0f, 0.0f, 0.0f, 0.0f });
	renderer_ = std::make_unique<ModelRenderer>(transform_.modelId, *material_);

	sinTime_ = 0.0f;
	offset_ = 0.1f + ((float)GetRand(5000)) / 1000.0f;

}

void Skull::Update(void)
{
}

void Skull::Draw(void)
{

	// 頂点：カメラ座標
	auto& camera = SceneManager::GetInstance().GetCamera();
	auto cPos = camera.GetPos();
	material_->SetConstBufVS(0, { cPos.x, cPos.y, cPos.z, 0.0f });


	auto delta = SceneManager::GetInstance().GetDeltaTime();
	sinTime_ += delta;
	material_->SetConstBufPS(3, { sinTime_, offset_, 0.0f, 0.0f });

	renderer_->Draw();

}

void Skull::SetPos(VECTOR pos)
{
	transform_.pos = pos;
	transform_.Update();
}

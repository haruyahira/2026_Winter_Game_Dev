#include "ModelRenderComponent.h"
#include "../../Manager/ResourceManager.h"
#include "../../Manager/SceneManager.h"
#include "../../Manager/Camera.h"
#include "../../Renderer/ModelMaterial.h"
#include "../../Renderer/ModelRenderer.h"

ModelRenderComponent::ModelRenderComponent(void)
	: imgToonMap_(-1)
	, material_(nullptr)
	, renderer_(nullptr)
{
}

ModelRenderComponent::~ModelRenderComponent(void)
{
}

void ModelRenderComponent::Setup(int& modelId, const std::string& vsName, const std::string& psName)
{
	auto& resMng = ResourceManager::GetInstance();
	imgToonMap_ = resMng.Load(ResourceManager::SRC::TEX_TOON).handleId_;

	material_ = std::make_unique<ModelMaterial>(vsName, 2, psName, 3);
	material_->SetTextureBuf(3, imgToonMap_);

	material_->AddConstBufPS({ 1.0f, 1.0f, 1.0f, 1.0f });
	material_->AddConstBufPS({ 0.2f, 0.2f, 0.2f, 1.0f });

	auto& camera = SceneManager::GetInstance().GetCamera();
	auto cPos = camera.GetPos();
	material_->AddConstBufVS({ cPos.x, cPos.y, cPos.z, 0.0f });

	float fogStart, fogEnd;
	GetFogStartEnd(&fogStart, &fogEnd);
	material_->AddConstBufVS({ fogStart, fogEnd, 0.0f, 0.0f });

	VECTOR worldLightDirection = GetLightDirection();
	material_->AddConstBufPS({ worldLightDirection.x, worldLightDirection.y, worldLightDirection.z, 0.0f });

	renderer_ = std::make_unique<ModelRenderer>(modelId, *material_);
}

void ModelRenderComponent::Draw(void)
{
	if (material_ && renderer_)
	{
		auto& camera = SceneManager::GetInstance().GetCamera();
		auto cameraPos = camera.GetPos();
		material_->SetConstBufVS(0, { cameraPos.x, cameraPos.y, cameraPos.z, 0.0f });

		renderer_->Draw();
	}
}

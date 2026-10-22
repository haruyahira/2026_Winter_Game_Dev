#pragma once
#include <memory>
#include <string>
#include <DxLib.h>
#include "../Component.h"

class ModelMaterial;
class ModelRenderer;

class ModelRenderComponent : public Component
{
public:
	ModelRenderComponent(void);
	~ModelRenderComponent(void) override;

	void Setup(int& modelId, const std::string& vsName, const std::string& psName);
	void Draw(void) override;

	ModelMaterial* GetMaterial(void) const { return material_.get(); }
	ModelRenderer* GetRenderer(void) const { return renderer_.get(); }

private:
	int imgToonMap_;
	std::unique_ptr<ModelMaterial> material_;
	std::unique_ptr<ModelRenderer> renderer_;
};

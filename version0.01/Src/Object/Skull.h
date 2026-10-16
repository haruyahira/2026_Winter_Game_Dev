#pragma once
#include "../Common/Quaternion.h"
#include "ActorBase.h"
class ModelMaterial;
class ModelRenderer;

class Skull 
	: public ActorBase
{

public :

	// コンストラクタ
	Skull(void);

	// デストラクタ
	~Skull(void);

	void Init(void) override;
	void Update(void) override;
	void Draw(void) override;

	void SetPos(VECTOR pos);

private:

	// モデル描画用用
	std::unique_ptr<ModelMaterial> material_;
	std::unique_ptr<ModelRenderer> renderer_;


	float sinTime_;
	float offset_;

};


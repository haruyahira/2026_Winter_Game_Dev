#include "ShadowRenderComponent.h"
#include <cmath>
#include "CapsuleColliderComponent.h"
#include "../ActorBase.h"
#include "../Common/Collider.h"
#include "../../Manager/ResourceManager.h"

ShadowRenderComponent::ShadowRenderComponent(void)
	: imgShadow_(-1)
	, shadowSize_(DEFAULT_SHADOW_SIZE)
	, shadowHeight_(DEFAULT_SHADOW_HEIGHT)
{
}

void ShadowRenderComponent::Setup(float shadowSize, float shadowHeight)
{
	shadowSize_ = shadowSize;
	shadowHeight_ = shadowHeight;
	imgShadow_ = ResourceManager::GetInstance().Load(ResourceManager::SRC::PLAYER_SHADOW).handleId_;
}

void ShadowRenderComponent::Draw(void)
{
	if (!owner_ || imgShadow_ == -1) return;

	auto colliderComp = owner_->GetComponent<CapsuleColliderComponent>();
	if (!colliderComp) return;

	const auto& colliders = colliderComp->GetColliders();
	const VECTOR& pos = owner_->transform_.pos;

	int i;
	MV1_COLL_RESULT_POLY_DIM HitResDim;
	MV1_COLL_RESULT_POLY* HitRes;
	VERTEX3D Vertex[3] = { VERTEX3D(), VERTEX3D(), VERTEX3D() };
	VECTOR SlideVec;
	int ModelHandle;

	SetUseLighting(FALSE);
	SetUseZBuffer3D(TRUE);
	SetTextureAddressMode(DX_TEXADDRESS_CLAMP);

	for (const auto& c : colliders)
	{
		if (c.expired()) continue;
		ModelHandle = c.lock()->modelId_;
		HitResDim = MV1CollCheck_Capsule(ModelHandle, -1, pos, VAdd(pos, { 0.0f, -shadowHeight_, 0.0f }), shadowSize_);

		Vertex[0].dif = GetColorU8(255, 255, 255, 255);
		Vertex[0].spc = GetColorU8(0, 0, 0, 0);
		Vertex[0].su = 0.0f; Vertex[0].sv = 0.0f;
		Vertex[1] = Vertex[0]; Vertex[2] = Vertex[0];

		HitRes = HitResDim.Dim;
		for (i = 0; i < HitResDim.HitNum; i++, HitRes++)
		{
			Vertex[0].pos = HitRes->Position[0];
			Vertex[1].pos = HitRes->Position[1];
			Vertex[2].pos = HitRes->Position[2];

			SlideVec = VScale(HitRes->Normal, 0.5f);
			Vertex[0].pos = VAdd(Vertex[0].pos, SlideVec);
			Vertex[1].pos = VAdd(Vertex[1].pos, SlideVec);
			Vertex[2].pos = VAdd(Vertex[2].pos, SlideVec);

			Vertex[0].dif.a = 0; Vertex[1].dif.a = 0; Vertex[2].dif.a = 0;
			if (HitRes->Position[0].y > pos.y - shadowHeight_) Vertex[0].dif.a = static_cast<int>(roundf(128.0f * (1.0f - fabs(HitRes->Position[0].y - pos.y) / shadowHeight_)));
			if (HitRes->Position[1].y > pos.y - shadowHeight_) Vertex[1].dif.a = static_cast<int>(roundf(128.0f * (1.0f - fabs(HitRes->Position[1].y - pos.y) / shadowHeight_)));
			if (HitRes->Position[2].y > pos.y - shadowHeight_) Vertex[2].dif.a = static_cast<int>(roundf(128.0f * (1.0f - fabs(HitRes->Position[2].y - pos.y) / shadowHeight_)));

			Vertex[0].u = (HitRes->Position[0].x - pos.x) / (shadowSize_ * 2.0f) + 0.5f; Vertex[0].v = (HitRes->Position[0].z - pos.z) / (shadowSize_ * 2.0f) + 0.5f;
			Vertex[1].u = (HitRes->Position[1].x - pos.x) / (shadowSize_ * 2.0f) + 0.5f; Vertex[1].v = (HitRes->Position[1].z - pos.z) / (shadowSize_ * 2.0f) + 0.5f;
			Vertex[2].u = (HitRes->Position[2].x - pos.x) / (shadowSize_ * 2.0f) + 0.5f; Vertex[2].v = (HitRes->Position[2].z - pos.z) / (shadowSize_ * 2.0f) + 0.5f;

			DrawPolygon3D(Vertex, 1, imgShadow_, TRUE);
		}
		MV1CollResultPolyDimTerminate(HitResDim);
	}
	SetUseLighting(TRUE);
	SetUseZBuffer3D(FALSE);
}

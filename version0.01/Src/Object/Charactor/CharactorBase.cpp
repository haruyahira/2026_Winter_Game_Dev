#include "CharactorBase.h"
#include "../../Manager/ResourceManager.h"
#include "../../Manager/SceneManager.h"
#include "../../Manager/Camera.h"
#include "../../Renderer/ModelRenderer.h"
#include "../../Renderer/ModelMaterial.h"
#include "../../Utility/AsoUtility.h"
#include "../Common/AnimationController.h"
#include "../Common/Collider.h"
#include "../Common/Capsule.h"
#include "../Planet.h" 

CharactorBase::CharactorBase(void)
{
	animationController_ = nullptr;
	material_ = nullptr;
	renderer_ = nullptr;
	capsule_ = nullptr;

	speed_ = 0.0f;
	moveDir_ = AsoUtility::VECTOR_ZERO;
	movePow_ = AsoUtility::VECTOR_ZERO;
	movedPos_ = AsoUtility::VECTOR_ZERO;
	jumpPow_ = AsoUtility::VECTOR_ZERO;
	isJump_ = false;

	gravHitPosDown_ = AsoUtility::VECTOR_ZERO;
	gravHitPosUp_ = AsoUtility::VECTOR_ZERO;

	imgShadow_ = -1;
	imgToonMap_ = -1;

	// ★追加: デフォルトのシェーダー設定
	vsName_ = "ToonModelVS6.cso";
	psName_ = "ToonModelPS6.cso";
}

CharactorBase::~CharactorBase(void)
{
}

void CharactorBase::Init(void)
{
	// 1. 子クラス(PlayerやEnemy)固有の初期化（モデル読み込みやカプセル設定）
	// ★ここで Centaur の場合は vsName_, psName_ が書き換えられる
	InitDetail();

	// 2. キャラクター共通の初期化処理（マテリアルやレンダラーの生成）
	auto& resMng = ResourceManager::GetInstance();

	imgShadow_ = resMng.Load(ResourceManager::SRC::PLAYER_SHADOW).handleId_;
	imgToonMap_ = resMng.Load(ResourceManager::SRC::TEX_TOON).handleId_;

	// ★修正: 変数を使用してマテリアルを生成
	material_ = std::make_unique<ModelMaterial>(vsName_, 2, psName_, 3);
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

	renderer_ = std::make_unique<ModelRenderer>(transform_.modelId, *material_);
}

void CharactorBase::AddCollider(std::weak_ptr<Collider> collider)
{
	colliders_.push_back(collider);
}

void CharactorBase::ClearCollider(void)
{
	colliders_.clear();
}

const Capsule& CharactorBase::GetCapsule(void) const
{
	return *capsule_;
}

void CharactorBase::DrawShadow(void)
{
	float PLAYER_SHADOW_HEIGHT = 300.0f;
	float PLAYER_SHADOW_SIZE = 30.0f;
	int i;
	MV1_COLL_RESULT_POLY_DIM HitResDim;
	MV1_COLL_RESULT_POLY* HitRes;
	VERTEX3D Vertex[3] = { VERTEX3D(), VERTEX3D(), VERTEX3D() };
	VECTOR SlideVec;
	int ModelHandle;

	SetUseLighting(FALSE);
	SetUseZBuffer3D(TRUE);
	SetTextureAddressMode(DX_TEXADDRESS_CLAMP);

	for (const auto c : colliders_)
	{
		ModelHandle = c.lock()->modelId_;
		HitResDim = MV1CollCheck_Capsule(ModelHandle, -1, transform_.pos, VAdd(transform_.pos, { 0.0f, -PLAYER_SHADOW_HEIGHT, 0.0f }), PLAYER_SHADOW_SIZE);

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
			if (HitRes->Position[0].y > transform_.pos.y - PLAYER_SHADOW_HEIGHT) Vertex[0].dif.a = static_cast<int>(roundf(128.0f * (1.0f - fabs(HitRes->Position[0].y - transform_.pos.y) / PLAYER_SHADOW_HEIGHT)));
			if (HitRes->Position[1].y > transform_.pos.y - PLAYER_SHADOW_HEIGHT) Vertex[1].dif.a = static_cast<int>(roundf(128.0f * (1.0f - fabs(HitRes->Position[1].y - transform_.pos.y) / PLAYER_SHADOW_HEIGHT)));
			if (HitRes->Position[2].y > transform_.pos.y - PLAYER_SHADOW_HEIGHT) Vertex[2].dif.a = static_cast<int>(roundf(128.0f * (1.0f - fabs(HitRes->Position[2].y - transform_.pos.y) / PLAYER_SHADOW_HEIGHT)));

			Vertex[0].u = (HitRes->Position[0].x - transform_.pos.x) / (PLAYER_SHADOW_SIZE * 2.0f) + 0.5f; Vertex[0].v = (HitRes->Position[0].z - transform_.pos.z) / (PLAYER_SHADOW_SIZE * 2.0f) + 0.5f;
			Vertex[1].u = (HitRes->Position[1].x - transform_.pos.x) / (PLAYER_SHADOW_SIZE * 2.0f) + 0.5f; Vertex[1].v = (HitRes->Position[1].z - transform_.pos.z) / (PLAYER_SHADOW_SIZE * 2.0f) + 0.5f;
			Vertex[2].u = (HitRes->Position[2].x - transform_.pos.x) / (PLAYER_SHADOW_SIZE * 2.0f) + 0.5f; Vertex[2].v = (HitRes->Position[2].z - transform_.pos.z) / (PLAYER_SHADOW_SIZE * 2.0f) + 0.5f;

			DrawPolygon3D(Vertex, 1, imgShadow_, TRUE);
		}
		MV1CollResultPolyDimTerminate(HitResDim);
	}
	SetUseLighting(TRUE);
	SetUseZBuffer3D(FALSE);
}

void CharactorBase::Collision(void)
{
	movedPos_ = VAdd(transform_.pos, movePow_);
	CollisionCapsule();
	CollisionGravity();
	transform_.pos = movedPos_;
}

void CharactorBase::CollisionCapsule(void)
{
	Transform trans = Transform(transform_);
	trans.pos = movedPos_;
	trans.Update();
	Capsule cap = Capsule(*capsule_, trans);

	for (const auto c : colliders_)
	{
		auto hits = MV1CollCheck_Capsule(c.lock()->modelId_, -1, cap.GetPosTop(), cap.GetPosDown(), cap.GetRadius());
		for (int i = 0; i < hits.HitNum; i++)
		{
			auto hit = hits.Dim[i];
			for (int tryCnt = 0; tryCnt < 10; tryCnt++)
			{
				int pHit = HitCheck_Capsule_Triangle(cap.GetPosTop(), cap.GetPosDown(), cap.GetRadius(), hit.Position[0], hit.Position[1], hit.Position[2]);
				if (pHit) {
					movedPos_ = VAdd(movedPos_, VScale(hit.Normal, 1.0f));
					trans.pos = movedPos_;
					trans.Update();
					continue;
				}
				break;
			}
		}
		MV1CollResultPolyDimTerminate(hits);
	}
}

void CharactorBase::CollisionGravity(void)
{
	movedPos_ = VAdd(movedPos_, jumpPow_);

	VECTOR dirGravity = AsoUtility::DIR_D;
	VECTOR dirUpGravity = AsoUtility::DIR_U;
	float gravityPow = Planet::DEFAULT_GRAVITY_POW;
	float checkPow = 10.0f;

	gravHitPosUp_ = VAdd(movedPos_, VScale(dirUpGravity, gravityPow));
	gravHitPosUp_ = VAdd(gravHitPosUp_, VScale(dirUpGravity, checkPow * 2.0f));
	gravHitPosDown_ = VAdd(movedPos_, VScale(dirGravity, checkPow));

	for (const auto c : colliders_)
	{
		auto hit = MV1CollCheck_Line(c.lock()->modelId_, -1, gravHitPosUp_, gravHitPosDown_);
		if (hit.HitFlag > 0 && VDot(dirGravity, jumpPow_) > 0.9f)
		{
			movedPos_ = VAdd(hit.HitPosition, VScale(dirUpGravity, 2.0f));
			jumpPow_ = AsoUtility::VECTOR_ZERO;

			// 派生クラスに着地を通知
			OnLanding();
		}
	}
}

void CharactorBase::CalcGravityPow(void)
{
	VECTOR dirGravity = AsoUtility::DIR_D;
	float gravityPow = Planet::DEFAULT_GRAVITY_POW;
	VECTOR gravity = VScale(dirGravity, gravityPow);

	jumpPow_ = VAdd(jumpPow_, gravity);
	float dot = VDot(dirGravity, jumpPow_);
	if (dot >= 0.0f)
	{
		jumpPow_ = gravity;
	}
}
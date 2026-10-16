#include "../Application.h"
#include "../Utility/AsoUtility.h"
#include "../Manager/ResourceManager.h"
#include "../Manager/Resource.h"
#include "../Manager/SceneManager.h"
#include "../Manager/Camera.h"
#include "../Renderer/ModelMaterial.h"
#include "../Renderer/ModelRenderer.h"
#include "Common/AnimationController.h"
#include "Common/Capsule.h"
#include "Common/Collider.h"
#include "Planet.h"
#include "Centaur.h"

Centaur::Centaur(void)
{
	imgShadow_ = -1;
}

Centaur::~Centaur(void)
{
}

void Centaur::Init(void)
{
	// モデルの基本設定
	transform_.SetModel(ResourceManager::GetInstance().LoadModelDuplicate(
		ResourceManager::SRC::CENTAUR));
	transform_.scl = VScale(AsoUtility::VECTOR_ONE, 1.5f);
	transform_.pos = { 150.0f, -40.0f, 200.0f };
	transform_.quaRot = Quaternion();
	transform_.quaRotLocal =
		Quaternion::Euler({ 0.0f, AsoUtility::Deg2RadF(300.0f), 0.0f });
	transform_.Update();

	// 髪
	MV1SetFrameVisible(transform_.modelId, 3, false);
	// 槍
	MV1SetFrameVisible(transform_.modelId, 132, false);

	// 丸影画像
	imgShadow_ = ResourceManager::GetInstance().Load(
		ResourceManager::SRC::PLAYER_SHADOW).handleId_;

	// トゥーンシェーダ用画像
	imgToonMap_ = ResourceManager::GetInstance().Load(
		ResourceManager::SRC::TEX_TOON).handleId_;

	// モデル描画用
	material_ = std::make_unique<ModelMaterial>(
		"ToonModelVS2.cso", 2,
		"ToonModelPS2.cso", 3
		);
	material_->SetTextureBuf(3, imgToonMap_);

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

	renderer_ = std::make_unique<ModelRenderer>(transform_.modelId, *material_);

	animationController_ = std::make_unique<AnimationController>(transform_.modelId);
	animationController_->AddInFBX((int)ANIM_TYPE::IDLE, 30.0f);
	animationController_->AddInFBX((int)ANIM_TYPE::WALK, 30.0f);

	animationController_->Play((int)ANIM_TYPE::IDLE);

	// カプセルコライダ
	capsule_ = std::make_unique<Capsule>(transform_);
	capsule_->SetLocalPosTop({ 0.0f, 110.0f, 0.0f });
	capsule_->SetLocalPosDown({ 0.0f, 30.0f, 0.0f });
	capsule_->SetRadius(20.0f);

	// 初期状態
	ChangeState(STATE::MOVE);
}

void Centaur::Update(void)
{
	switch (state_)
	{
	case Centaur::STATE::NONE:
		break;
	case Centaur::STATE::THINK:
		UpdateThink();
		break;
	case Centaur::STATE::IDLE:
		UpdateIdle();
		break;
	case Centaur::STATE::MOVE:
		UpdateMove();
		break;
	case Centaur::STATE::ACTION:
		UpdateAction();
		break;
	case Centaur::STATE::END:
		break;
	default:
		break;
	}

	// 重力による移動量
	CalcGravityPow();

	// 衝突判定
	Collision();

	transform_.Update();

	// アニメーション再生
	animationController_->Update();
}

void Centaur::Draw(void)
{
	// 頂点：カメラ座標
	auto& camera = SceneManager::GetInstance().GetCamera();
	auto cPos = camera.GetPos();
	material_->SetConstBufVS(0, { cPos.x, cPos.y, cPos.z, 0.0f });

	// モデルの描画
	renderer_->Draw();

	// 丸影描画
	DrawShadow();
}

void Centaur::AddCollider(std::weak_ptr<Collider> collider)
{
	colliders_.push_back(collider);
}

void Centaur::ClearCollider(void)
{
	colliders_.clear();
}

void Centaur::ChangeState(STATE state)
{
	state_ = state;

	step_ = 0.0f;
	movePow_ = VECTOR();

	switch (state_)
	{
	case Centaur::STATE::NONE:
		break;
	case Centaur::STATE::THINK:
	{
		// 2～3
		int tmpState = GetRand(2) + 2;
		ChangeState((STATE)tmpState);
	}
	break;
	case Centaur::STATE::IDLE:
		animationController_->Play((int)ANIM_TYPE::IDLE);
		step_ = (float)(2000 + GetRand(3000)) / 1000.0f;
		break;
	case Centaur::STATE::MOVE:
	{
		animationController_->Play((int)ANIM_TYPE::WALK);
		step_ = (float)(3000 + GetRand(3000)) / 1000.0f;
		moveDir_ = VECTOR();
		moveDir_.x = ((float)(GetRand(2000)) / 1000.0f) - 1.0f;
		moveDir_.y = 0.0f;
		moveDir_.z = ((float)(GetRand(2000)) / 1000.0f) - 1.0f;
		moveDir_ = VNorm(moveDir_);
	}
	break;
	case Centaur::STATE::ACTION:
		break;
	case Centaur::STATE::END:
		break;
	default:
		break;
	}
}

void Centaur::UpdateThink(void)
{
}

void Centaur::UpdateIdle(void)
{
	step_ -= SceneManager::GetInstance().GetDeltaTime();
	if (step_ < 0.0f)
	{
		ChangeState(STATE::THINK);
	}
}

void Centaur::UpdateMove(void)
{
	movePow_ = VScale(moveDir_, 2.0f);

	// 行先チェック
	auto testPos = VAdd(transform_.pos, VScale(movePow_, 5.0f));

	// 重力方向
	VECTOR dirGravity = AsoUtility::DIR_D;

	// 重力方向の反対
	VECTOR dirUpGravity = AsoUtility::DIR_U;

	// 重力の強さ
	float gravityPow = Planet::DEFAULT_GRAVITY_POW;

	float checkPow = 10.0f;
	auto gravHitPosUp_ = VAdd(testPos, VScale(dirUpGravity, gravityPow));
	gravHitPosUp_ = VAdd(gravHitPosUp_, VScale(dirUpGravity, checkPow * 2.0f));
	auto gravHitPosDown_ = VAdd(testPos, VScale(dirGravity, checkPow * 10.0f));
	for (const auto c : colliders_)
	{

		// 地面との衝突
		auto hit = MV1CollCheck_Line(
			c.lock()->modelId_, -1, gravHitPosUp_, gravHitPosDown_);

		if (hit.HitFlag == 0)
		{
			moveDir_ = VScale(moveDir_, -1.0f);
			movePow_ = VScale(moveDir_, 4.0f);
		}

	}

	transform_.quaRot = Quaternion::Slerp(
		transform_.quaRot,
		Quaternion::LookRotation(moveDir_), 0.1f);

	step_ -= SceneManager::GetInstance().GetDeltaTime();
	if (step_ < 0.0f)
	{
		ChangeState(STATE::THINK);
	}
}

void Centaur::UpdateAction(void)
{
	step_ -= SceneManager::GetInstance().GetDeltaTime();
	if (step_ < 0.0f)
	{
		ChangeState(STATE::THINK);
	}
}

void Centaur::DrawShadow(void)
{
	float PLAYER_SHADOW_HEIGHT = 300.0f;
	float PLAYER_SHADOW_SIZE = 100.0f;

	int i;
	MV1_COLL_RESULT_POLY_DIM HitResDim;
	MV1_COLL_RESULT_POLY* HitRes;
	VERTEX3D Vertex[3] = { VERTEX3D(), VERTEX3D(), VERTEX3D() };
	VECTOR SlideVec;
	int ModelHandle;

	// ライティングを無効にする
	SetUseLighting(FALSE);

	// Ｚバッファを有効にする
	SetUseZBuffer3D(TRUE);

	// テクスチャアドレスモードを CLAMP にする( テクスチャの端より先は端のドットが延々続く )
	SetTextureAddressMode(DX_TEXADDRESS_CLAMP);

	// 影を落とすモデルの数だけ繰り返し
	for (const auto c : colliders_)
	{

		// チェックするモデルは、jが0の時はステージモデル、1以上の場合はコリジョンモデル
		ModelHandle = c.lock()->modelId_;

		// プレイヤーの直下に存在する地面のポリゴンを取得
		HitResDim = MV1CollCheck_Capsule(
			ModelHandle, -1,
			transform_.pos, VAdd(transform_.pos, { 0.0f, -PLAYER_SHADOW_HEIGHT, 0.0f }), PLAYER_SHADOW_SIZE);

		// 頂点データで変化が無い部分をセット
		Vertex[0].dif = GetColorU8(255, 255, 255, 255);
		Vertex[0].spc = GetColorU8(0, 0, 0, 0);
		Vertex[0].su = 0.0f;
		Vertex[0].sv = 0.0f;
		Vertex[1] = Vertex[0];
		Vertex[2] = Vertex[0];

		// 球の直下に存在するポリゴンの数だけ繰り返し
		HitRes = HitResDim.Dim;
		for (i = 0; i < HitResDim.HitNum; i++, HitRes++)
		{
			// ポリゴンの座標は地面ポリゴンの座標
			Vertex[0].pos = HitRes->Position[0];
			Vertex[1].pos = HitRes->Position[1];
			Vertex[2].pos = HitRes->Position[2];

			// ちょっと持ち上げて重ならないようにする
			SlideVec = VScale(HitRes->Normal, 0.5f);
			Vertex[0].pos = VAdd(Vertex[0].pos, SlideVec);
			Vertex[1].pos = VAdd(Vertex[1].pos, SlideVec);
			Vertex[2].pos = VAdd(Vertex[2].pos, SlideVec);

			// ポリゴンの不透明度を設定する
			Vertex[0].dif.a = 0;
			Vertex[1].dif.a = 0;
			Vertex[2].dif.a = 0;
			if (HitRes->Position[0].y > transform_.pos.y - PLAYER_SHADOW_HEIGHT)
				Vertex[0].dif.a = static_cast<int>(roundf(128.0f * (1.0f - fabs(HitRes->Position[0].y - transform_.pos.y) / PLAYER_SHADOW_HEIGHT)));

			if (HitRes->Position[1].y > transform_.pos.y - PLAYER_SHADOW_HEIGHT)
				Vertex[1].dif.a = static_cast<int>(roundf(128.0f * (1.0f - fabs(HitRes->Position[1].y - transform_.pos.y) / PLAYER_SHADOW_HEIGHT)));

			if (HitRes->Position[2].y > transform_.pos.y - PLAYER_SHADOW_HEIGHT)
				Vertex[2].dif.a = static_cast<int>(roundf(128.0f * (1.0f - fabs(HitRes->Position[2].y - transform_.pos.y) / PLAYER_SHADOW_HEIGHT)));

			// ＵＶ値は地面ポリゴンとプレイヤーの相対座標から割り出す
			Vertex[0].u = (HitRes->Position[0].x - transform_.pos.x) / (PLAYER_SHADOW_SIZE * 2.0f) + 0.5f;
			Vertex[0].v = (HitRes->Position[0].z - transform_.pos.z) / (PLAYER_SHADOW_SIZE * 2.0f) + 0.5f;
			Vertex[1].u = (HitRes->Position[1].x - transform_.pos.x) / (PLAYER_SHADOW_SIZE * 2.0f) + 0.5f;
			Vertex[1].v = (HitRes->Position[1].z - transform_.pos.z) / (PLAYER_SHADOW_SIZE * 2.0f) + 0.5f;
			Vertex[2].u = (HitRes->Position[2].x - transform_.pos.x) / (PLAYER_SHADOW_SIZE * 2.0f) + 0.5f;
			Vertex[2].v = (HitRes->Position[2].z - transform_.pos.z) / (PLAYER_SHADOW_SIZE * 2.0f) + 0.5f;

			// 影ポリゴンを描画
			DrawPolygon3D(Vertex, 1, imgShadow_, TRUE);
		}

		// 検出した地面ポリゴン情報の後始末
		MV1CollResultPolyDimTerminate(HitResDim);
	}

	// ライティングを有効にする
	SetUseLighting(TRUE);

	// Ｚバッファを無効にする
	SetUseZBuffer3D(FALSE);
}

void Centaur::Collision(void)
{
	// 現在座標を起点に移動後座標を決める
	movedPos_ = VAdd(transform_.pos, movePow_);

	// 衝突(カプセル)
	CollisionCapsule();

	// 衝突(重力)
	CollisionGravity();

	// 移動
	transform_.pos = movedPos_;
}

void Centaur::CollisionGravity(void)
{
	// ジャンプ量を加算
	movedPos_ = VAdd(movedPos_, jumpPow_);

	// 重力方向
	VECTOR dirGravity = AsoUtility::DIR_D;

	// 重力方向の反対
	VECTOR dirUpGravity = AsoUtility::DIR_U;

	// 重力の強さ
	float gravityPow = Planet::DEFAULT_GRAVITY_POW;

	float checkPow = 10.0f;
	auto gravHitPosUp = VAdd(movedPos_, VScale(dirUpGravity, gravityPow));
	gravHitPosUp = VAdd(gravHitPosUp, VScale(dirUpGravity, checkPow * 2.0f));
	auto gravHitPosDown = VAdd(movedPos_, VScale(dirGravity, checkPow));
	for (const auto c : colliders_)
	{

		// 地面との衝突
		auto hit = MV1CollCheck_Line(
			c.lock()->modelId_, -1, gravHitPosUp, gravHitPosDown);

		// 最初は上の行のように実装して、木の上に登ってしまうことを確認する
		//if (hit.HitFlag > 0)
		if (hit.HitFlag > 0 && VDot(dirGravity, jumpPow_) > 0.9f)
		{

			// 衝突地点から、少し上に移動
			movedPos_ = VAdd(hit.HitPosition, VScale(dirUpGravity, 2.0f));

			// ジャンプリセット
			jumpPow_ = AsoUtility::VECTOR_ZERO;

		}

	}
}

void Centaur::CollisionCapsule(void)
{

	// カプセルを移動させる
	Transform trans = Transform(transform_);
	trans.pos = movedPos_;
	trans.Update();
	Capsule cap = Capsule(*capsule_, trans);

	// カプセルとの衝突判定
	for (const auto c : colliders_)
	{

		auto hits = MV1CollCheck_Capsule(
			c.lock()->modelId_, -1,
			cap.GetPosTop(), cap.GetPosDown(), cap.GetRadius());

		for (int i = 0; i < hits.HitNum; i++)
		{
			auto hit = hits.Dim[i];

			for (int tryCnt = 0; tryCnt < 10; tryCnt++)
			{
				int pHit = HitCheck_Capsule_Triangle(
					cap.GetPosTop(), cap.GetPosDown(), cap.GetRadius(),
					hit.Position[0], hit.Position[1], hit.Position[2]);

				if (pHit)
				{
					movedPos_ = VAdd(movedPos_, VScale(hit.Normal, 1.0f));
					// カプセルを移動させる
					trans.pos = movedPos_;
					trans.Update();
					continue;
				}

				break;

			}
		}

		// 検出した地面ポリゴン情報の後始末
		MV1CollResultPolyDimTerminate(hits);

	}
}

void Centaur::CalcGravityPow(void)
{

	// 重力方向
	VECTOR dirGravity = AsoUtility::DIR_D;

	// 重力の強さ
	float gravityPow = Planet::DEFAULT_GRAVITY_POW;

	// 重力
	VECTOR gravity = VScale(dirGravity, gravityPow);
	jumpPow_ = VAdd(jumpPow_, gravity);

	// 最初は実装しない。地面と突き抜けることを確認する。
	// 内積
	float dot = VDot(dirGravity, jumpPow_);
	if (dot >= 0.0f)
	{
		// 重力方向と反対方向(マイナス)でなければ、ジャンプ力を無くす
		jumpPow_ = gravity;
	}

}

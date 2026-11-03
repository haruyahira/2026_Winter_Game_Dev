// Src/Object/Components/PhysicsComponent.cpp
#include "PhysicsComponent.h"
#include "CapsuleColliderComponent.h"
#include "../ActorBase.h"
#include "../Common/Capsule.h"
#include "../Common/Collider.h"
#include "../Planet.h"
#include "../../Manager/SceneManager.h"
#include "../../Utility/AsoUtility.h"

PhysicsComponent::PhysicsComponent(void)
	: speed_(0.0f)
	, moveDir_(AsoUtility::VECTOR_ZERO)
	, movePow_(AsoUtility::VECTOR_ZERO)
	, movedPos_(AsoUtility::VECTOR_ZERO)
	, jumpPow_(AsoUtility::VECTOR_ZERO)
	, isJump_(false)
	, gravHitPosDown_(AsoUtility::VECTOR_ZERO)
	, gravHitPosUp_(AsoUtility::VECTOR_ZERO)
	, isGravityEnabled_(true)
	, onLanding_(nullptr)
{
}

void PhysicsComponent::Update(void)
{
	if (!owner_) return;

	if (isGravityEnabled_)
	{
		CalcGravityPow();
		Collision();
	}

	owner_->transform_.Update();
}

void PhysicsComponent::CalcGravityPow(void)
{
	VECTOR dirGravity = AsoUtility::DIR_D;
	float gravityPow = Planet::DEFAULT_GRAVITY_POW;

	float deltaTime = SceneManager::GetInstance().GetDeltaTime();
	if (deltaTime > 0.025f) {
		deltaTime = 0.025f;
	}
	float dtScale = deltaTime * 60.0f;
	VECTOR gravity = VScale(dirGravity, gravityPow * dtScale);

	// ジャンプ力（落下速度）に重力を足して加速させる
	jumpPow_ = VAdd(jumpPow_, gravity);

	float dot = VDot(dirGravity, jumpPow_);
	// 速度のリセットをやめ、「一定速度以上は速くならない（終端速度）」ようにする
	if (dot > 60.0f) // 30.0fは最大落下速度です。ゲームに合わせて調整してください
	{
		jumpPow_ = VScale(dirGravity, 60.0f);
	}
}

void PhysicsComponent::Collision(void)
{
	if (!owner_) return;

	// movedPos_の初期計算はCollisionCapsuleの中で（分割して）行う
	CollisionCapsule();
	CollisionGravity();

	owner_->transform_.pos = movedPos_;
}

void PhysicsComponent::CollisionCapsule(void)
{
	if (!owner_) return;
	auto colliderComp = owner_->GetComponent<CapsuleColliderComponent>();
	if (!colliderComp || !colliderComp->GetCapsulePtr())
	{
		// コライダーが無い場合はそのまま移動
		movedPos_ = VAdd(owner_->transform_.pos, movePow_);
		return;
	}

	Capsule baseCap = colliderComp->GetCapsule();
	float radius = baseCap.GetRadius();

	// 【連続衝突判定（CCD）】移動ベクトルを安全な長さに分割する
	float moveLen = VSize(movePow_);
	float safeStep = radius * 0.5f;
	if (safeStep < 1.0f) safeStep = 1.0f;

	int stepCount = (int)(moveLen / safeStep) + 1;
	VECTOR stepMove = VScale(movePow_, 1.0f / (float)stepCount);

	VECTOR currentPos = owner_->transform_.pos;

	// 分割した回数だけループ（1フレームの中で少しずつ進んで判定する）
	for (int step = 0; step < stepCount; ++step)
	{
		currentPos = VAdd(currentPos, stepMove);

		for (const auto& c : colliderComp->GetColliders())
		{
			if (c.expired()) continue;

			// ★修正：外側の判定用のカプセルをここで新しく作る
			Transform trans = Transform(owner_->transform_);
			trans.pos = currentPos;
			trans.Update();
			Capsule stepCap(baseCap, trans);

			auto hits = MV1CollCheck_Capsule(c.lock()->modelId_, -1, stepCap.GetPosTop(), stepCap.GetPosDown(), radius);

			for (int i = 0; i < hits.HitNum; i++)
			{
				auto hit = hits.Dim[i];

				// 内積（Dot）を利用した数学的な壁スライド計算
				VECTOR normal = VNorm(hit.Normal);
				float dot = VDot(stepMove, normal);

				if (dot < 0.0f)
				{
					// 壁に沿うベクトルに変換する
					stepMove = VSub(stepMove, VScale(normal, dot));
				}

				// すでにめり込んでしまった分の押し戻し処理
				for (int tryCnt = 0; tryCnt < 10; tryCnt++)
				{
					// ★修正：エラーの原因だった「上書き代入(=)」をやめ、ループ内で毎回新しいカプセルを作る
					Transform checkTrans = Transform(owner_->transform_);
					checkTrans.pos = currentPos;
					checkTrans.Update();
					Capsule checkCap(baseCap, checkTrans);

					int pHit = HitCheck_Capsule_Triangle(checkCap.GetPosTop(), checkCap.GetPosDown(), radius, hit.Position[0], hit.Position[1], hit.Position[2]);
					if (pHit)
					{
						currentPos = VAdd(currentPos, VScale(normal, 1.0f));
					}
					else
					{
						break; // めり込みが解消されたら押し戻し完了
					}
				}
			}
			MV1CollResultPolyDimTerminate(hits);
		}
	}

	// 最終的に計算された安全な位置を movedPos_ にセット
	movedPos_ = currentPos;
}



//void PhysicsComponent::CollisionGravity(void)
//{
//	if (!owner_) return;
//	auto colliderComp = owner_->GetComponent<CapsuleColliderComponent>();
//	if (!colliderComp) return;
//
//	// ★ 修正：ここでも同じように上限を設ける
//	float deltaTime = SceneManager::GetInstance().GetDeltaTime();
//	if (deltaTime > 0.025f) {
//		deltaTime = 0.025f;
//	}
//	float dtScale = deltaTime * 60.0f;
//	movedPos_ = VAdd(movedPos_, VScale(jumpPow_, dtScale));
//
//	VECTOR dirGravity = AsoUtility::DIR_D;
//	VECTOR dirUpGravity = AsoUtility::DIR_U;
//	float gravityPow = Planet::DEFAULT_GRAVITY_POW;
//	float checkPow = 10.0f;
//
//	gravHitPosUp_ = VAdd(movedPos_, VScale(dirUpGravity, gravityPow));
//	gravHitPosUp_ = VAdd(gravHitPosUp_, VScale(dirUpGravity, checkPow * 2.0f));
//	gravHitPosDown_ = VAdd(movedPos_, VScale(dirGravity, checkPow));
//
//	for (const auto& c : colliderComp->GetColliders())
//	{
//		if (c.expired()) continue;
//		auto hit = MV1CollCheck_Line(c.lock()->modelId_, -1, gravHitPosUp_, gravHitPosDown_);
//		if (hit.HitFlag > 0 && VDot(dirGravity, jumpPow_) > 0.0f)
//		{
//			movedPos_ = VAdd(hit.HitPosition, VScale(dirUpGravity, 2.0f));
//			jumpPow_ = AsoUtility::VECTOR_ZERO;
//
//			if (onLanding_)
//			{
//				onLanding_();
//			}
//		}
//	}
//}

void PhysicsComponent::CollisionGravity(void)
{
	if (!owner_) return;
	auto colliderComp = owner_->GetComponent<CapsuleColliderComponent>();
	if (!colliderComp) return;

	float deltaTime = SceneManager::GetInstance().GetDeltaTime();
	if (deltaTime > 0.025f) {
		deltaTime = 0.025f;
	}
	float dtScale = deltaTime * 60.0f;

	// ★修正ポイント1：落下する「前」の座標を記憶しておく
	VECTOR oldPos = movedPos_;

	// 重力による移動を適用
	movedPos_ = VAdd(movedPos_, VScale(jumpPow_, dtScale));

	VECTOR dirGravity = AsoUtility::DIR_D;
	VECTOR dirUpGravity = AsoUtility::DIR_U;
	float checkPow = 10.0f; // めり込み防止用の足元の余裕

	// ★修正ポイント2：レイ（線分）を飛ばす始点を「落下前の位置」、終点を「落下後の位置」にする
	// これにより、どれだけ落下速度が速くても途中の床を絶対にすり抜けない
	gravHitPosUp_ = VAdd(oldPos, VScale(dirUpGravity, checkPow)); // 移動前の少し上から
	gravHitPosDown_ = VAdd(movedPos_, VScale(dirGravity, checkPow)); // 移動後の少し下まで線を引く

	for (const auto& c : colliderComp->GetColliders())
	{
		if (c.expired()) continue;
		auto hit = MV1CollCheck_Line(c.lock()->modelId_, -1, gravHitPosUp_, gravHitPosDown_);

		// 落下中（下に向かっている）かつ、床にぶつかった場合
		if (hit.HitFlag == 1 && VDot(dirGravity, jumpPow_) > 0.0f)
		{
			// 床の高さにピタッと止める（めり込まないよう、ほんの少し上に浮かせる）
			movedPos_ = VAdd(hit.HitPosition, VScale(dirUpGravity, 1.0f));
			jumpPow_ = AsoUtility::VECTOR_ZERO;

			if (onLanding_)
			{
				onLanding_();
			}
			break; // 一度床に当たったらループ終了
		}
	}
}
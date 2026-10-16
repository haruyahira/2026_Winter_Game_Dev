#include "EnemyBase.h"
#include "../../../Utility/AsoUtility.h"
#include "../../../Manager/SceneManager.h"
#include "../../../Manager/Camera.h"
#include "../../../Renderer/ModelRenderer.h"
#include "../../../Renderer/ModelMaterial.h"
#include "../../Common/AnimationController.h"


// ※もしPlayerの座標を取得して追いかける処理を書く場合は、ここでPlayer.hをインクルードします
// #include "../Player/Player.h"

EnemyBase::EnemyBase(void)
{
	state_ = STATE::IDLE;
}

EnemyBase::~EnemyBase(void)
{
}

void EnemyBase::Update(void)
{
	// AIの思考（プレイヤーを追いかける方向を計算するなど）
	ProcessAI();

	// 物理演算（重力と当たり判定
	CalcGravityPow();
	Collision();

	// 座標とアニメーションの更新
	transform_.Update();
	if (animationController_)
	{
		animationController_->Update();
	}
}

void EnemyBase::Draw(void)
{
	// 描画はPlayerとほぼ同じ流れ
	if (material_ && renderer_)
	{
		auto& camera = SceneManager::GetInstance().GetCamera();
		auto cPos = camera.GetPos();
		material_->SetConstBufVS(0, { cPos.x, cPos.y, cPos.z, 0.0f });

		renderer_->Draw();
	}

	// 丸影の描画 ※CharactorBaseの機能
	DrawShadow();
}

void EnemyBase::SetTarget(std::weak_ptr<Player> target)
{
	// プレイヤーの情報を記憶する
	target_ = target;
}

void EnemyBase::ProcessAI(void)
{
	// 敵の基本的なAIロジックをここに書きます
	// 例：target_（プレイヤー）の座標と自分の座標を引き算して、近づくベクトルを作る など
	// 今回は枠組みだけ用意しておきます
}

void EnemyBase::OnLanding(void)
{
	// 着地した時の処理
	isJump_ = false;

	// もし敵専用の着地アニメーションがあればここで再生する
}
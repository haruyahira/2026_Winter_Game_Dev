#include "FireBall.h"
#include "../../Manager/Camera.h"
#include "../../Manager/ResourceManager.h"
#include "../../Manager/SceneManager.h"
#include "../../Utility/AsoUtility.h"

FireBall::FireBall(void)
    : moveDir_(AsoUtility::VECTOR_ZERO)
    , lifeTimer_(5.0f) // 2秒間飛んだら消える
{
    damage_ = 20;
    speed_ = 10.0f; // 1フレームあたりの移動速度
}

FireBall::~FireBall(void)
{
}

void FireBall::Init(void)
{
    // FIREBALL モデルを読み込む
    transform_.SetModel(ResourceManager::GetInstance().LoadModelDuplicate(
        ResourceManager::SRC::FIREBALL));

    // 大きさを設定（もし見えなければ VGet(10.0f, 10.0f, 10.0f) などに調整してください）
    transform_.scl = AsoUtility::VECTOR_ONE;
}

void FireBall::Update(void)
{
    if (!isAlive_) return;

    VECTOR movePow = VScale(moveDir_, speed_);
    transform_.pos = VAdd(transform_.pos, movePow);
    transform_.Update();

    lifeTimer_ -= 1.0f / 60.0f;

    if (lifeTimer_ <= 0.0f)
    {
        Destroy();
    }
}

void FireBall::Draw(void)
{
    if (!isAlive_) return;

    // ★ 独自シェーダー（ボーン用）は使わず、標準機能で描画する！
    MV1DrawModel(transform_.modelId);
}

bool FireBall::isDestroy(void)
{
    return !isAlive_;
}

void FireBall::Fire(const Transform& spawnTransform, VECTOR dir)
{
    transform_.pos = spawnTransform.pos;
    moveDir_ = VNorm(dir);
    transform_.quaRot = Quaternion::LookRotation(moveDir_);
    transform_.Update();
    isAlive_ = true;
}
#include "MagicBase.h"

MagicBase::MagicBase(void)
    : damage_(0)
    , speed_(0.0f)
    , collisionRadius_(25.0f)
    , isAlive_(false)
{
}

MagicBase::~MagicBase(void)
{
}

void MagicBase::Fire(const Transform& spawnTransform, VECTOR dir)
{
    // 撃った人（プレイヤーや敵）の位置と向きを魔法の初期値にセット
    transform_ = spawnTransform;
    isAlive_ = true;
}
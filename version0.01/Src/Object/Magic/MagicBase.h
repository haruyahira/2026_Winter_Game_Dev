#pragma once
#include "../ActorBase.h"

class MagicBase : public ActorBase
{
public:

    MagicBase(void);
    virtual ~MagicBase(void);

    virtual void Update(void) = 0;
    virtual void Draw(void) = 0;

    // 魔法を発射する（撃ったキャラクターの座標や向きを受け取る）
    virtual void Fire(const Transform& spawnTransform, VECTOR dir);

    // 敵などに当たった瞬間のはじける処理
    virtual void OnHit(void) { Destroy(); }

    // ゲッター
    int GetDamage(void) const { return damage_; }
    float GetCollisionRadius(void) const { return collisionRadius_; }
    bool IsAlive(void) const { return isAlive_; }
    virtual bool CanHit(void) const { return isAlive_; }
    void Destroy(void) { isAlive_ = false; }  // 消滅させる

protected:

    int damage_;            // 威力
    float speed_;           // 飛んでいく速さ
    float collisionRadius_; // 当たり判定の半径
    bool isAlive_;          // 生存フラグ
};
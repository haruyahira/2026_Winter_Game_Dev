#pragma once
#include "../ActorBase.h"

class MagicBase : public ActorBase
{
public:

    MagicBase(void);
    virtual ~MagicBase(void);

    // ActorBaseにある想定の純粋仮想関数（または仮想関数）をオーバーライド
    virtual void Update(void) = 0;
    virtual void Draw(void) = 0;

    // 魔法を発射する（撃ったキャラクターの座標や向きを受け取る）
    virtual void Fire(const Transform& spawnTransform, VECTOR dir);

    // ゲッター
    int GetDamage(void) const { return damage_; }
    bool IsAlive(void) const { return isAlive_; }
    void Destroy(void) { isAlive_ = false; }  // ヒット時に呼んで消滅させる

protected:

    int damage_;    // 威力
    float speed_;   // 飛んでいく速さ
    bool isAlive_;  // 生存フラグ（敵や壁に当たったら消すため）
};
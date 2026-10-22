// Component.h
#pragma once

class ActorBase; // 前方宣言（インクルードエラーを防ぐため）

class Component {
protected:
    // 自分がくっついている本体（箱）のポインタ
    ActorBase* owner_ = nullptr;

public:
    virtual ~Component() = default;

    // 本体をセットする
    void SetOwner(ActorBase* owner) { owner_ = owner; }

    // 本体を取得する
    ActorBase* GetOwner() const { return owner_; }

    virtual void Update() {}
    virtual void Draw() {}
};
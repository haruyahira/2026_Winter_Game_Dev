<<<<<<< HEAD
#pragma once

class ActorBase;

class Component
{
public:
	Component(void) = default;
	virtual ~Component(void) = default;

	// この部品の持ち主（本体）を設定・取得
	void SetOwner(ActorBase* owner) { owner_ = owner; }
	ActorBase* GetOwner(void) const { return owner_; }

	virtual void Init(void) {}
	virtual void Update(void) {}
	virtual void Draw(void) {}

	void SetActive(bool isActive) { isActive_ = isActive; }
	bool IsActive(void) const { return isActive_; }

protected:
	ActorBase* owner_ = nullptr;
	bool isActive_ = true;
};
=======
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
>>>>>>> f9ee5eae87468ea5e2cf6eeaa42714623e2e46bd

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

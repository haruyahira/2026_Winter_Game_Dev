#pragma once
#include <vector>
#include <memory>
#include "Component.h"
#include "Common/Transform.h"

class ResourceManager;
class SceneManager;

class ActorBase
{
public:
	// コンストラクタ
	ActorBase(void);

	// デストラクタ
	virtual ~ActorBase(void);

	virtual void Init(void) = 0;
	virtual void Update(void) = 0;
	virtual void Draw(void) = 0;

	const Transform& GetTransform(void) const;

	virtual bool isDestroy(void) { return false; }

	// 部品（コンポーネント）を取り付ける関数
	void AddComponent(std::shared_ptr<Component> component)
	{
		component->SetOwner(this);
		component->Init();
		components_.push_back(component);
	}

	// 欲しい部品を探して取り出すテンプレート関数
	template <class T>
	T* GetComponent(void) const
	{
		for (const auto& c : components_)
		{
			if (auto typed = dynamic_cast<T*>(c.get()))
			{
				return typed;
			}
		}
		return nullptr;
	}

	// 取り付けられている全部品のUpdateを呼ぶ
	virtual void UpdateComponents(void)
	{
		for (auto& c : components_)
		{
			if (c && c->IsActive())
			{
				c->Update();
			}
		}
	}

	// 取り付けられている全部品のDrawを呼ぶ
	virtual void DrawComponents(void)
	{
		for (auto& c : components_)
		{
			if (c && c->IsActive())
			{
				c->Draw();
			}
		}
	}

	// モデル制御の基本情報（コンポーネントからアクセス可能にするためpublicに配置）
	Transform transform_;

private:

	int test;

protected:

	// シングルトン参照
	ResourceManager& resMng_;
	SceneManager& scnMng_;

	// 所持しているコンポーネントのリスト
	std::vector<std::shared_ptr<Component>> components_;

};

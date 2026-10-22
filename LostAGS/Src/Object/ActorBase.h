// ActorBase.h に追記・修正
#pragma once
#include <vector>
#include <memory>
#include "Component.h"
<<<<<<< HEAD
#include "Common/Transform.h"
=======
#include "Common/Transform.h" // 既存のインクルード
>>>>>>> f9ee5eae87468ea5e2cf6eeaa42714623e2e46bd

class ResourceManager;
class SceneManager;

<<<<<<< HEAD
class ActorBase
{
=======
class ActorBase {
>>>>>>> f9ee5eae87468ea5e2cf6eeaa42714623e2e46bd
public:
    // ★ ここに移動させます（コンポーネントから直接アクセスして座標などを書き換えるため）
    Transform transform_;

<<<<<<< HEAD
	// デストラクタ
	virtual ~ActorBase(void);
=======
    ActorBase(void);
    virtual ~ActorBase(void);
>>>>>>> f9ee5eae87468ea5e2cf6eeaa42714623e2e46bd

    const Transform& GetTransform(void) const;

    // 部品を取り付ける関数
    void AddComponent(std::shared_ptr<Component> component) {
        component->SetOwner(this);
        components_.push_back(component);
    }

    // 欲しい部品を探して取り出すテンプレート関数 (超重要！)
    template <class T>
    T* GetComponent() {
        for (auto& c : components_) {
            // dynamic_cast で型が一致したものを返す
            if (auto typed = dynamic_cast<T*>(c.get())) {
                return typed;
            }
        }
        return nullptr; // 見つからなかったら nullptr
    }

<<<<<<< HEAD
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
=======
    // 全部品のUpdateを呼ぶ（必要なら派生クラスで呼ぶ）
    virtual void UpdateComponents() {
        for (auto& c : components_) {
            c->Update();
        }
    }
>>>>>>> f9ee5eae87468ea5e2cf6eeaa42714623e2e46bd

protected:
    ResourceManager& resMng_;
    SceneManager& scnMng_;

<<<<<<< HEAD
	// シングルトン参照
	ResourceManager& resMng_;
	SceneManager& scnMng_;

	// 所持しているコンポーネントのリスト
	std::vector<std::shared_ptr<Component>> components_;

};
=======
   
    std::vector<std::shared_ptr<Component>> components_;
};
>>>>>>> f9ee5eae87468ea5e2cf6eeaa42714623e2e46bd

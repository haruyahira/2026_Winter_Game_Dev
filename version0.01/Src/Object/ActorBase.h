// ActorBase.h に追記・修正
#pragma once
#include <vector>
#include <memory>
#include "Component.h"
#include "Common/Transform.h" // 既存のインクルード

class ResourceManager;
class SceneManager;

class ActorBase {
public:
    // ★ ここに移動させます（コンポーネントから直接アクセスして座標などを書き換えるため）
    Transform transform_;

    ActorBase(void);
    virtual ~ActorBase(void);

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

    // 全部品のUpdateを呼ぶ（必要なら派生クラスで呼ぶ）
    virtual void UpdateComponents() {
        for (auto& c : components_) {
            c->Update();
        }
    }

protected:
    ResourceManager& resMng_;
    SceneManager& scnMng_;

   
    std::vector<std::shared_ptr<Component>> components_;
};
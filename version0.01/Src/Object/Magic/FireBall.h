#pragma once
#include "MagicBase.h"
#include <memory>
// ★追加：専用の描画システム
#include "../../Renderer/ModelMaterial.h"
#include "../../Renderer/ModelRenderer.h"

class FireBall : public MagicBase
{
public:
    FireBall(void);
    virtual ~FireBall(void);

    void Init(void) override;
    void Update(void) override;
    void Draw(void) override;
    void Fire(const Transform& spawnTransform, VECTOR dir) override;
    bool isDestroy(void) override;

private:
    VECTOR moveDir_;
    float lifeTimer_;

    // ★追加：シェーダーとレンダラー
    std::unique_ptr<ModelMaterial> material_;
    std::unique_ptr<ModelRenderer> renderer_;
};
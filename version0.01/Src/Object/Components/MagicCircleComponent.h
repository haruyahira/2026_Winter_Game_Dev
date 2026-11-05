#pragma once
#include <string>
#include <DxLib.h>
#include "../Magic/MagicData.h" // ※パスは環境に合わせて調整してください

struct ShaderParam
{
    float progress;
    float edgeWidth;
    float dummy[2];
    float edgeColor[4];
};

struct VS_ShaderParam
{
    MATRIX matView;
    MATRIX matProj;
};

class MagicCircleComponent
{
public:
    MagicCircleComponent();
    ~MagicCircleComponent();

    void Init();

    // 描画には「何の魔法か」と「詠唱がどのくらい進んでいるか」だけを受け取る
    void Draw(const std::string& spellName, float currentCastTime);
    void DrawAtFeet(VECTOR playerPos, float time);

private:
    // 魔法陣用のリソース
    int magicBaseImg_;
    int magicMaskImg_;
    int pixelShader_;
    int vertexShader_;
    int constBuffer_;
    int vsConstBuffer_;
};
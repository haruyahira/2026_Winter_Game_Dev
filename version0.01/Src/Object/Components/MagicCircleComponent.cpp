#include "MagicCircleComponent.h"
#include "../../Manager/Camera.h" // mainCamera を使うため（パスは調整してください）
#include "../../Manager/SceneManager.h"

MagicCircleComponent::MagicCircleComponent()
    : magicBaseImg_(-1), magicMaskImg_(-1)
    , pixelShader_(-1), vertexShader_(-1)
    , constBuffer_(-1), vsConstBuffer_(-1)
{
}

MagicCircleComponent::~MagicCircleComponent()
{
    if (magicBaseImg_ != -1) DeleteGraph(magicBaseImg_);
    if (magicMaskImg_ != -1) DeleteGraph(magicMaskImg_);
    if (pixelShader_ != -1) DeleteShader(pixelShader_);
    if (vertexShader_ != -1) DeleteShader(vertexShader_);
    if (constBuffer_ != -1) DeleteShaderConstantBuffer(constBuffer_);
    if (vsConstBuffer_ != -1) DeleteShaderConstantBuffer(vsConstBuffer_);
}

void MagicCircleComponent::Init()
{
    magicBaseImg_ = LoadGraph("Data/Image/MagicCircle.png");
    magicMaskImg_ = LoadGraph("Data/Image/MaskSpiral.png");

    pixelShader_ = LoadPixelShader("Data/Shader/MagicCirclePS.cso");
    vertexShader_ = LoadVertexShader("Data/Shader/MagicCircleVS.cso");

    constBuffer_ = CreateShaderConstantBuffer(sizeof(ShaderParam));
    vsConstBuffer_ = CreateShaderConstantBuffer(sizeof(VS_ShaderParam));
}

void MagicCircleComponent::Draw(const std::string& spellName, float currentCastTime)
{
    if (!MagicDatabase::IsValidSpell(spellName)) return;

    MagicData mData = MagicDatabase::GetData(spellName);

    VECTOR cPos, cTarget, cUp, cDir, cRight;

    cPos = mainCamera.GetPos();
    cTarget = mainCamera.GetTargetPos();
    cDir = VNorm(VSub(cTarget, cPos));

    VECTOR tempUp = VGet(0.0f, 1.0f, 0.0f);
    cRight = VNorm(VCross(tempUp, cDir));
    cUp = VNorm(VCross(cDir, cRight));

    float rotAngle = currentCastTime * DX_PI_F * 0.1f;

    MATRIX rotMat = MGetRotAxis(cDir, rotAngle);
    cRight = VTransform(cRight, rotMat);
    cUp = VTransform(cUp, rotMat);

    float forwardOffset = mData.circleDistance;
    VECTOR centerPos = VAdd(cPos, VScale(cDir, forwardOffset));

    float size = mData.circleSize;
    VECTOR vTR = VAdd(centerPos, VAdd(VScale(cRight, size), VScale(cUp, size)));
    VECTOR vTL = VAdd(centerPos, VAdd(VScale(cRight, -size), VScale(cUp, size)));
    VECTOR vBR = VAdd(centerPos, VAdd(VScale(cRight, size), VScale(cUp, -size)));
    VECTOR vBL = VAdd(centerPos, VAdd(VScale(cRight, -size), VScale(cUp, -size)));

    VERTEX3DSHADER vert[6];
    memset(vert, 0, sizeof(vert));
    int r, g, b;
    GetColor2(mData.circleColor, &r, &g, &b);
    COLOR_U8 col = GetColorU8(r, g, b, 255);

    for (int i = 0; i < 6; i++)
    {
        vert[i].spos = { 0.0f, 0.0f, 0.0f, 1.0f };
        vert[i].norm = VGet(0.0f, 0.0f, -1.0f);
        vert[i].tan = VGet(1.0f, 0.0f, 0.0f);
        vert[i].binorm = VGet(0.0f, 1.0f, 0.0f);
        vert[i].dif = col;
        vert[i].spc = GetColorU8(0, 0, 0, 0);
    }

    vert[0].pos = vTL; vert[0].u = 0.0f; vert[0].v = 0.0f; vert[0].su = 0.0f; vert[0].sv = 0.0f;
    vert[1].pos = vTR; vert[1].u = 1.0f; vert[1].v = 0.0f; vert[1].su = 1.0f; vert[1].sv = 0.0f;
    vert[2].pos = vBL; vert[2].u = 0.0f; vert[2].v = 1.0f; vert[2].su = 0.0f; vert[2].sv = 1.0f;
    vert[3].pos = vTR; vert[3].u = 1.0f; vert[3].v = 0.0f; vert[3].su = 1.0f; vert[3].sv = 0.0f;
    vert[4].pos = vBR; vert[4].u = 1.0f; vert[4].v = 1.0f; vert[4].su = 1.0f; vert[4].sv = 1.0f;
    vert[5].pos = vBL; vert[5].u = 0.0f; vert[5].v = 1.0f; vert[5].su = 0.0f; vert[5].sv = 1.0f;

    SetUseLighting(FALSE);
    SetUseZBuffer3D(TRUE);
    SetWriteZBuffer3D(FALSE);
    SetDrawBlendMode(DX_BLENDMODE_ADD, 255);
    SetUseBackCulling(FALSE);

    VS_ShaderParam* vsParam = static_cast<VS_ShaderParam*>(GetBufferShaderConstantBuffer(vsConstBuffer_));
    vsParam->matView = GetCameraViewMatrix();
    vsParam->matProj = GetCameraProjectionMatrix();
    UpdateShaderConstantBuffer(vsConstBuffer_);

    SetUseVertexShader(vertexShader_);
    SetShaderConstantBuffer(vsConstBuffer_, DX_SHADERTYPE_VERTEX, 2);

    float progress = currentCastTime / mData.castTime;
    if (progress > 0.99f) progress = 0.99f;

    ShaderParam* param = static_cast<ShaderParam*>(GetBufferShaderConstantBuffer(constBuffer_));
    param->progress = progress;
    param->edgeWidth = 0.08f;
    param->edgeColor[0] = r / 255.0f;
    param->edgeColor[1] = g / 255.0f;
    param->edgeColor[2] = b / 255.0f;
    param->edgeColor[3] = 1.0f;
    UpdateShaderConstantBuffer(constBuffer_);

    SetUsePixelShader(pixelShader_);
    SetShaderConstantBuffer(constBuffer_, DX_SHADERTYPE_PIXEL, 1);

    SetUseTextureToShader(0, magicBaseImg_);
    SetUseTextureToShader(1, magicMaskImg_);

    DrawPolygon3DToShader(vert, 2);

    SetUsePixelShader(-1);
    SetUseVertexShader(-1);
    SetUseTextureToShader(0, -1);
    SetUseTextureToShader(1, -1);

    SetUseBackCulling(TRUE);
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
    SetWriteZBuffer3D(TRUE);
    SetUseLighting(TRUE);
}

void MagicCircleComponent::DrawAtFeet(VECTOR playerPos, float time)
{
    // 回転速度とサイズ（お好みで調整してください）
    float rotAngle = time * DX_PI_F * 0.5f;
    float size = 150.0f; // 足元用のサイズ

    // 水平（X, Z軸）に広がるように計算
    VECTOR cRight = VGet(1.0f, 0.0f, 0.0f);
    VECTOR cFront = VGet(0.0f, 0.0f, 1.0f);

    MATRIX rotMat = MGetRotY(rotAngle);
    cRight = VTransform(cRight, rotMat);
    cFront = VTransform(cFront, rotMat);

    // プレイヤーの足元（地面とのチラつきを防ぐため少しだけ上に浮かせる）
    VECTOR centerPos = playerPos;
    centerPos.y += 0.5f;

    VECTOR vTR = VAdd(centerPos, VAdd(VScale(cRight, size), VScale(cFront, size)));
    VECTOR vTL = VAdd(centerPos, VAdd(VScale(cRight, -size), VScale(cFront, size)));
    VECTOR vBR = VAdd(centerPos, VAdd(VScale(cRight, size), VScale(cFront, -size)));
    VECTOR vBL = VAdd(centerPos, VAdd(VScale(cRight, -size), VScale(cFront, -size)));

    VERTEX3DSHADER vert[6];
    memset(vert, 0, sizeof(vert));

    // 飛行用魔法陣の色（例：風や空をイメージした薄い緑〜水色）
    COLOR_U8 col = GetColorU8(150, 255, 200, 255);

    for (int i = 0; i < 6; i++)
    {
        vert[i].spos = { 0.0f, 0.0f, 0.0f, 1.0f };
        vert[i].norm = VGet(0.0f, 1.0f, 0.0f); // 真上を向く
        vert[i].tan = VGet(1.0f, 0.0f, 0.0f);
        vert[i].binorm = VGet(0.0f, 0.0f, 1.0f);
        vert[i].dif = col;
        vert[i].spc = GetColorU8(0, 0, 0, 0);
    }

    vert[0].pos = vTL; vert[0].u = 0.0f; vert[0].v = 0.0f; vert[0].su = 0.0f; vert[0].sv = 0.0f;
    vert[1].pos = vTR; vert[1].u = 1.0f; vert[1].v = 0.0f; vert[1].su = 1.0f; vert[1].sv = 0.0f;
    vert[2].pos = vBL; vert[2].u = 0.0f; vert[2].v = 1.0f; vert[2].su = 0.0f; vert[2].sv = 1.0f;
    vert[3].pos = vTR; vert[3].u = 1.0f; vert[3].v = 0.0f; vert[3].su = 1.0f; vert[3].sv = 0.0f;
    vert[4].pos = vBR; vert[4].u = 1.0f; vert[4].v = 1.0f; vert[4].su = 1.0f; vert[4].sv = 1.0f;
    vert[5].pos = vBL; vert[5].u = 0.0f; vert[5].v = 1.0f; vert[5].su = 0.0f; vert[5].sv = 1.0f;

    SetUseLighting(FALSE);
    SetUseZBuffer3D(TRUE);
    SetWriteZBuffer3D(FALSE);
    SetDrawBlendMode(DX_BLENDMODE_ADD, 255);
    SetUseBackCulling(FALSE);

    VS_ShaderParam* vsParam = static_cast<VS_ShaderParam*>(GetBufferShaderConstantBuffer(vsConstBuffer_));
    vsParam->matView = GetCameraViewMatrix();
    vsParam->matProj = GetCameraProjectionMatrix();
    UpdateShaderConstantBuffer(vsConstBuffer_);

    SetUseVertexShader(vertexShader_);
    SetShaderConstantBuffer(vsConstBuffer_, DX_SHADERTYPE_VERTEX, 2);

    ShaderParam* param = static_cast<ShaderParam*>(GetBufferShaderConstantBuffer(constBuffer_));
    param->progress = 1.0f; // 足元の魔法陣は常に100%展開状態
    param->edgeWidth = 0.08f;
    param->edgeColor[0] = 0.2f;
    param->edgeColor[1] = 0.8f;
    param->edgeColor[2] = 1.0f;
    param->edgeColor[3] = 1.0f;
    UpdateShaderConstantBuffer(constBuffer_);

    SetUsePixelShader(pixelShader_);
    SetShaderConstantBuffer(constBuffer_, DX_SHADERTYPE_PIXEL, 1);
    SetUseTextureToShader(0, magicBaseImg_);
    SetUseTextureToShader(1, magicMaskImg_);

    DrawPolygon3DToShader(vert, 2);

    SetUsePixelShader(-1);
    SetUseVertexShader(-1);
    SetUseTextureToShader(0, -1);
    SetUseTextureToShader(1, -1);

    SetUseBackCulling(TRUE);
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
    SetWriteZBuffer3D(TRUE);
    SetUseLighting(TRUE);
}
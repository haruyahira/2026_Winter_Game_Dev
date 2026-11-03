#include <cmath>
#include "ZoltraakMagic.h"
#include "../../Manager/SceneManager.h"
#include "../../Manager/InputManager.h"
#include "../../Manager/Camera.h"

 
ZoltraakMagic::ZoltraakMagic(const MagicData& data)
    : GenericMagic(data)
    , pixelShaderHandle_(-1)
    , noiseTextureHandle_(-1)
    , cbufferHandle_(-1)
    , currentTime_(0.0f)
    , beamSpeed_(4.0f)
    , fireDir_(VGet(0.0f, 0.0f, 1.0f))
{
}

ZoltraakMagic::~ZoltraakMagic()
{
    if (pixelShaderHandle_ != -1) DeleteShader(pixelShaderHandle_);
    if (noiseTextureHandle_ != -1) DeleteGraph(noiseTextureHandle_);
    if (cbufferHandle_ != -1) DeleteShaderConstantBuffer(cbufferHandle_);
}

void ZoltraakMagic::Init()
{
    GenericMagic::Init();
    collisionRadius_ = 80.0f;

    pixelShaderHandle_ = LoadPixelShader("Data/Shader/ZoltraakBeamPS.cso");
    noiseTextureHandle_ = LoadGraph("Data/Image/Noise.png");
    cbufferHandle_ = CreateShaderConstantBuffer(sizeof(BeamParamCBuffer));
}

void ZoltraakMagic::Fire(const Transform& spawnTransform, VECTOR dir)
{
    GenericMagic::Fire(spawnTransform, dir);
    transform_.quaRot = spawnTransform.quaRot;
    fireDir_ = dir;
}

void ZoltraakMagic::Update()
{
    GenericMagic::Update();

    if (isAlive_)
    {
        currentTime_ += SceneManager::GetInstance().GetDeltaTime();

        // フリーカメラ中かどうかの分岐を消去し、常に mainCamera を使う！
        VECTOR cPos = mainCamera.GetPos();
        VECTOR cTarget = mainCamera.GetTargetPos();
        VECTOR cDir = VNorm(VSub(cTarget, cPos));

        // 当たり判定の球を、根本から先端まで超高速で往復（スキャン）させる
        float scanDist = SPAWN_DISTANCE + std::fmod(currentTime_ * END_DISTANCE * 2, END_DISTANCE);

        transform_.pos = VAdd(cPos, VScale(cDir, scanDist));
        fireDir_ = cDir;
        transform_.quaRot = mainCamera.GetQuaRot();

        auto& ins = InputManager::GetInstance();
        if (currentTime_ > 0.3f &&
            (ins.IsTrgMouseLeft() || CheckHitKey(KEY_INPUT_RETURN) || ins.IsTrgDown(KEY_INPUT_TAB)))
        {
            Destroy();
        }
    }
}

void ZoltraakMagic::DrawBeamLayer(VECTOR pos, VECTOR scale, FLOAT4 color, int blendMode, int blendParam)
{
   
    VECTOR localOffset = VGet(0.0f, -20.5f, 0.0f);
    MATRIX matOffset = MGetTranslate(localOffset);

    MATRIX matScl = MGetScale(scale);
    MATRIX matRot = transform_.quaRot.GetMatrix();
    MATRIX matPos = MGetTranslate(pos);

    // 順番は絶対に「ズレ補正」→「拡大」→「回転」→「移動」
    MATRIX matWorld = MMult(matOffset, matScl);
    matWorld = MMult(matWorld, matRot);
    matWorld = MMult(matWorld, matPos);

    MV1SetMatrix(transform_.modelId, matWorld);

    // ダミー描画によるシェーダーリセット
    MV1SetUseOrigShader(FALSE);
    SetUseVertexShader(-1);
    SetUsePixelShader(-1);
    SetDrawBlendMode(DX_BLENDMODE_ALPHA, 0);
    MV1DrawModel(transform_.modelId);

    // 本番描画
    if (pixelShaderHandle_ != -1 && cbufferHandle_ != -1)
    {
        BeamParamCBuffer* cb = static_cast<BeamParamCBuffer*>(GetBufferShaderConstantBuffer(cbufferHandle_));
        cb->time = currentTime_;
        cb->beamSpeed = beamSpeed_;
        cb->dummy[0] = 0.0f;
        cb->dummy[1] = 0.0f;
        cb->auraColor = color;
        UpdateShaderConstantBuffer(cbufferHandle_);

        SetShaderConstantBuffer(cbufferHandle_, DX_SHADERTYPE_PIXEL, 3);
        SetUsePixelShader(pixelShaderHandle_);
        if (noiseTextureHandle_ != -1)
        {
            SetUseTextureToShader(1, noiseTextureHandle_);
        }
        MV1SetUseOrigShader(TRUE);
    }

    COLOR_F colF = GetColorF(color.x, color.y, color.z, color.w);
    MV1SetDifColorScale(transform_.modelId, colF);
    MV1SetEmiColorScale(transform_.modelId, colF);

    SetDrawBlendMode(blendMode, blendParam);
    MV1DrawModel(transform_.modelId);

    MV1SetUseOrigShader(FALSE);
}

void ZoltraakMagic::Draw()
{
    if (!isAlive_)
    {
        GenericMagic::Draw();
        return;
    }

    VECTOR cPos = mainCamera.GetPos();
    VECTOR cTarget = mainCamera.GetTargetPos();
    VECTOR cDir = VNorm(VSub(cTarget, cPos));

    // 魔法のスタート地点
    float spawnDistance = SPAWN_DISTANCE;
    VECTOR beamCenterPos = VAdd(cPos, VScale(cDir, spawnDistance));

    SetUseLighting(FALSE);
    SetUseZBuffer3D(TRUE);
    SetWriteZBuffer3D(FALSE);
    MV1SetUseZBuffer(transform_.modelId, TRUE);
    MV1SetWriteZBuffer(transform_.modelId, FALSE);
    SetUseBackCulling(FALSE);

    // 第1層：周囲を囲うαオーラ
    DrawBeamLayer(
        beamCenterPos,
        VGet(13.0f, 13.0f, 100.0f),
        { 2.0f, 2.0f, 2.0f, 0.3f },
        DX_BLENDMODE_ALPHA, 140
    );

    // 第2層：中間の発光オーラ
    DrawBeamLayer(
        beamCenterPos,
        VGet(10.0f, 10.0f, 100.0f),
        { 1.0f, 1.0f, 1.0f, 0.7f },
        DX_BLENDMODE_ADD, 200
    );

    // 第2層：中間の発光オーラ
    DrawBeamLayer(
        beamCenterPos,
        VGet(8.0f, 8.0f, 100.0f),
        { 2.0f, 2.0f, 2.0f, 0.85f },
        DX_BLENDMODE_ADD, 200
    );

    // 一番内側
    DrawBeamLayer(
        beamCenterPos,
        VGet(6.0f, 6.0f, 100.0f),
        { 3.0f, 3.0f, 3.0f, 1.0f },
        DX_BLENDMODE_ADD, 255
    );

    MV1SetWriteZBuffer(transform_.modelId, TRUE);
    SetUseBackCulling(TRUE);
    SetWriteZBuffer3D(TRUE);
    SetUseLighting(TRUE);
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
    SetUseTextureToShader(1, -1);
    SetShaderConstantBuffer(-1, DX_SHADERTYPE_PIXEL, 3);
    SetUsePixelShader(-1);
    SetUseVertexShader(-1);

    SetUseZBuffer3D(FALSE);
    SetWriteZBuffer3D(FALSE);

    DrawSphere3D(
        transform_.pos,
        collisionRadius_,
        16,
        GetColor(255, 0, 0),
        GetColor(255, 255, 255),
        FALSE
    );

    // 元の描画設定に戻す
    SetUseZBuffer3D(TRUE);
    SetWriteZBuffer3D(TRUE);
    SetUseLighting(TRUE);
}
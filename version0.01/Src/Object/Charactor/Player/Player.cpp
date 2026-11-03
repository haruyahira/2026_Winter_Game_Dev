// Src/Object/Charactor/Player/Player.cpp
#include "Player.h"
#include <string>
#include "../../../Application.h"
#include "../../../Manager/Camera.h"
#include "../../../Manager/InputManager.h"
#include "../../../Manager/ResourceManager.h"
#include "../../../Manager/SceneManager.h"
#include "../../Components/AnimationComponent.h"
#include "../../Components/PlayerControlComponent.h"
#include "../../Magic/GenericMagic.h" 
#include "../../Magic/MagicData.h" 

Player::Player(void)
    : controlComp_(nullptr)
    , isCasting_(false)
    , currentCastTime_(0.0f)
{
}

Player::~Player(void)
{
    DeleteGraph(magicBaseImg_);
    DeleteGraph(magicMaskImg_);
    DeleteShader(pixelShader_);
    DeleteShader(vertexShader_);
    DeleteShaderConstantBuffer(constBuffer_);
    DeleteShaderConstantBuffer(vsConstBuffer_);
}

void Player::InitDetail(void)
{
    LoadAndSetupModel(ResourceManager::SRC::PLAYER, INIT_SCALE, INIT_POSITION, INIT_AXIS);
    SetupCapsule(CAPSULE_TOP, CAPSULE_BOTTOM, CAPSULE_RADIUS);
    InitAnimation();

    controlComp_ = std::make_shared<PlayerControlComponent>();
    if (spawnActorFunc_)
    {
        controlComp_->SetSpawnFunc(spawnActorFunc_);
    }
    AddComponent(controlComp_);

    magicBaseImg_ = LoadGraph("Data/Image/MagicCircle.png");
    magicMaskImg_ = LoadGraph("Data/Image/MaskSpiral.png");

    pixelShader_ = LoadPixelShader("Data/Shader/MagicCirclePS.cso");
    vertexShader_ = LoadVertexShader("Data/Shader/MagicCircleVS.cso");

    constBuffer_ = CreateShaderConstantBuffer(sizeof(ShaderParam));
    vsConstBuffer_ = CreateShaderConstantBuffer(sizeof(VS_ShaderParam));
}

void Player::InitAnimation(void)
{
    if (!animComp_) return;
    std::string path = Application::PATH_MODEL + "Player/";
    animComp_->Add((int)ANIM_TYPE::IDLE, path + "Idle.mv1", 20.0f);
    animComp_->Add((int)ANIM_TYPE::RUN, path + "Run.mv1", 20.0f);
    animComp_->Add((int)ANIM_TYPE::FAST_RUN, path + "FastRun.mv1", 20.0f);
    animComp_->Add((int)ANIM_TYPE::JUMP, path + "Jump.mv1", 60.0f);
    animComp_->Add((int)ANIM_TYPE::WARP_PAUSE, path + "WarpPose.mv1", 60.0f);
    animComp_->Add((int)ANIM_TYPE::FLY, path + "Flying.mv1", 60.0f);
    animComp_->Add((int)ANIM_TYPE::FALLING, path + "Falling.mv1", 80.0f);
    animComp_->Add((int)ANIM_TYPE::VICTORY, path + "Victory.mv1", 60.0f);

    animComp_->Play((int)ANIM_TYPE::IDLE);
}

void Player::Update(void)
{
    CharactorBase::Update();

    if (isCasting_)
    {
        MagicData mData = MagicDatabase::GetData(castingSpell_);
        float dt = SceneManager::GetInstance().GetDeltaTime();

        if (currentCastTime_ < mData.castTime)
        {
            currentCastTime_ += dt;
            if (currentCastTime_ >= mData.castTime)
            {
                if (controlComp_) controlComp_->ProcessMagic(castingSpell_);
                if (mData.name != "zoltraak") isCasting_ = false;
            }
        }
        else
        {
            currentCastTime_ += dt;
            auto& ins = InputManager::GetInstance();

            // フリーカメラ中(mainCamera非アクティブ)はクリックしても魔法陣が消えないようにする
            if (mainCamera.IsActive() && (currentCastTime_ - mData.castTime) > 0.3f &&
                (ins.IsTrgMouseLeft() || CheckHitKey(KEY_INPUT_RETURN) || ins.IsTrgDown(KEY_INPUT_TAB)))
            {
                isCasting_ = false;
            }
        }
    }
}

void Player::Draw()
{
    CharactorBase::Draw();

    if (isCasting_)
    {
        VECTOR cPos, cTarget, cUp, cDir, cRight;

        // フリーカメラかどうかの判定を消去し、常にプレイヤーのメインカメラを基準にする
        cPos = mainCamera.GetPos();
        cTarget = mainCamera.GetTargetPos();
        cDir = VNorm(VSub(cTarget, cPos));

        // 仮の真上ベクトルを使って、まずは「右方向（cRight）」を計算する
        VECTOR tempUp = VGet(0.0f, 1.0f, 0.0f);
        cRight = VNorm(VCross(tempUp, cDir));

        // 前方向（cDir）と 右方向（cRight）を使って、「本当の傾いた上方向（cUp）」を計算し直す！
        cUp = VNorm(VCross(cDir, cRight));

        // ビームの開始位置と完全に合わせる
        float forwardOffset = 400.0f;
        VECTOR centerPos = VAdd(cPos, VScale(cDir, forwardOffset));

        float size = 400.0f; // 少し奥になった分、スケールを大きめに調整
        VECTOR vTR = VAdd(centerPos, VAdd(VScale(cRight, size), VScale(cUp, size)));
        VECTOR vTL = VAdd(centerPos, VAdd(VScale(cRight, -size), VScale(cUp, size)));
        VECTOR vBR = VAdd(centerPos, VAdd(VScale(cRight, size), VScale(cUp, -size)));
        VECTOR vBL = VAdd(centerPos, VAdd(VScale(cRight, -size), VScale(cUp, -size)));

        VERTEX3DSHADER vert[6];
        memset(vert, 0, sizeof(vert));
        COLOR_U8 col = GetColorU8(255, 255, 255, 255);

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
        // ★ここだけは GetCameraViewMatrix (現在画面を映しているカメラの行列) で正解です！
        vsParam->matView = GetCameraViewMatrix();
        vsParam->matProj = GetCameraProjectionMatrix();
        UpdateShaderConstantBuffer(vsConstBuffer_);

        SetUseVertexShader(vertexShader_);
        SetShaderConstantBuffer(vsConstBuffer_, DX_SHADERTYPE_VERTEX, 2);

        MagicData mData = MagicDatabase::GetData(castingSpell_);
        float progress = currentCastTime_ / mData.castTime;
        if (progress > 0.99f) progress = 0.99f;

        ShaderParam* param = static_cast<ShaderParam*>(GetBufferShaderConstantBuffer(constBuffer_));
        param->progress = progress;
        param->edgeWidth = 0.08f;
        param->edgeColor[0] = mData.circleColor[0];
        param->edgeColor[1] = mData.circleColor[1];
        param->edgeColor[2] = mData.circleColor[2];
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
}

void Player::StartCasting(const std::string& spellName)
{
    if (MagicDatabase::IsValidSpell(spellName))
    {
        isCasting_ = true;
        currentCastTime_ = 0.0f;
        castingSpell_ = spellName;
    }
}

void Player::SetSpawnFunc(std::function<void(std::unique_ptr<ActorBase>)> func)
{
    spawnActorFunc_ = func;
    if (controlComp_) controlComp_->SetSpawnFunc(func);
}

void Player::OnLanding(void)
{
    if (controlComp_) controlComp_->OnLanding();
}
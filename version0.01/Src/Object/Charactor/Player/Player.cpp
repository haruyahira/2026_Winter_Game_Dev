#include "Player.h"
#include <string>
#include "../../../Application.h"
#include "../../../Manager/Camera.h"
#include "../../../Manager/InputManager.h"
#include "../../../Manager/ResourceManager.h"
#include "../../../Manager/SceneManager.h"
#include "../../Components/AnimationComponent.h"
#include "../../Components/MagicCircleComponent.h"
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

    magicCircleComp_ = std::make_shared<MagicCircleComponent>();

    magicCircleComp_->Init();
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

            // ƒtƒŠ[ƒJƒƒ‰’†‚ÍƒNƒŠƒbƒN‚µ‚Ä‚à–‚–@w‚ªÁ‚¦‚È‚¢‚æ‚¤‚É‚·‚é
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

    // ’Êí‚Ì‰r¥Žž‚Ì–‚–@w
    if (isCasting_ && magicCircleComp_)
    {
        magicCircleComp_->Draw(castingSpell_, currentCastTime_);
    }

    // ”òs’†‚È‚ç‘«Œ³‚É–‚–@w‚ð•`‰æ
    if (controlComp_ && controlComp_->GetState() == PlayerControlComponent::STATE::FLY && magicCircleComp_)
    {
        magicCircleComp_->DrawAtFeet(transform_.pos, controlComp_->GetFlyTime());
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
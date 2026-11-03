#pragma once
#include "GenericMagic.h"
#include <DxLib.h>

struct BeamParamCBuffer
{
    float time;
    float beamSpeed;
    float dummy[2];
    FLOAT4 auraColor;
};

class ZoltraakMagic : public GenericMagic
{
public:

    ZoltraakMagic(const MagicData& data);
    virtual ~ZoltraakMagic();

    void Init() override;
    void Update() override;
    void Draw() override;
    void Fire(const Transform& spawnTransform, VECTOR dir) override;

private:

    const float SPAWN_DISTANCE = 420.0f;
    const float END_DISTANCE = 50000.0f;
    void DrawBeamLayer(VECTOR pos, VECTOR scale, FLOAT4 color, int blendMode, int blendParam);

    int pixelShaderHandle_;
    int noiseTextureHandle_;
    int cbufferHandle_;

    float currentTime_;
    float beamSpeed_;
    VECTOR fireDir_; // ”­ŽË•ûŒü
};
// VS/PS共通
#include "../Common/VertexToPixelHeader.hlsli"

// IN
#define PS_INPUT VertexToPixelLitFog

// PS
#include "../Common/Pixel/PixelShader3DHeader.hlsli"

// ① 基本パラメータ（レジスタ b4）※既存のまま
cbuffer cbParam : register(b4)
{
    float4 g_diff_color;
    float4 g_ambient_color;
    float3 g_light_dir;
    float dummy1;

    float g_time;
    float g_offset;
    float dummy2;
    float dummy3;
}

// ② ポイントライト環境パラメータ（レジスタ b5）
#define MAX_POINT_LIGHT 32
#define MAX_SPOT_LIGHT  8   // ★追加

struct PointLightData
{
    float3 pos;
    float range;
    float4 color;
};

// ★追加：C++側とサイズを一致させるためのダミー構造体
struct SpotLightData
{
    float3 pos;
    float range;
    float3 direction;
    float innerConeCos;
    float4 color;
    float outerConeCos;
    float3 padding;
};

cbuffer cbLightEnv : register(b5)
{
    PointLightData g_pointLights[MAX_POINT_LIGHT];
    int g_pointLightCount;
    float3 g_pad1;

    // ★追加：C++側とサイズを一致させるためのダミー枠
    SpotLightData g_spotLights[MAX_SPOT_LIGHT];
    int g_spotLightCount;
    float3 g_pad2;
};

// ====================================================
// メイン関数
// ====================================================
// ====================================================
// メイン関数
// ====================================================
// ====================================================
// メイン関数 (PointLightPS.hlsl)
// ====================================================
float4 main(PS_INPUT PSInput) : SV_TARGET0
{
    // テクスチャーの色を取得
    float4 color = diffuseMapTexture.Sample(diffuseMapSampler, PSInput.uv);
    if (color.a < 0.01f)
    {
        discard;
    }
    
    // ベースライト計算（平行光源）
    float lightDot = max(dot(PSInput.normal, -g_light_dir), 0.0f);
    float3 totalLight = lightDot * g_diff_color.rgb;
    
    float3 pos = PSInput.worldPos;

    // ポイントライトの加算
    for (int i = 0; i < g_pointLightCount; i++)
    {
        float3 lightVec = g_pointLights[i].pos - pos;
        float distance = length(lightVec);
        
        if (distance < g_pointLights[i].range)
        {
            float3 lightDir = normalize(lightVec);
            
            // 距離減衰（遠くの床まで柔らかく光が届くように二乗減衰にする）
            float atten = saturate(1.0f - (distance / g_pointLights[i].range));
            float attenuation = atten * atten;
            
            // ハーフランバート（角度が浅い床面にも確実に光を回す）
            float nDotL = dot(PSInput.normal, lightDir) * 0.5f + 0.5f;
            
            totalLight += g_pointLights[i].color.rgb * nDotL * attenuation;
        }
    }

    // 環境光を足し合わせる
    float3 finalLight = totalLight + g_ambient_color.rgb;
    float3 rgb = color.rgb * finalLight;

    // ★重要：巨大ポリゴン（床）が真っ黒になる原因の頂点フォグを無効化する！
    // rgb *= PSInput.fogFactor;
    
    return float4(rgb, color.a);
}
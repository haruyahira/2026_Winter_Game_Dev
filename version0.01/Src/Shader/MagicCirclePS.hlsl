struct PS_INPUT
{
    float4 pos : SV_POSITION;
    float4 dif : COLOR0;
    float4 spc : COLOR1;
    float2 uv : TEXCOORD0;
    float2 su : TEXCOORD1;
};

cbuffer cbParam : register(b1)
{
    float progress;
    float edgeWidth;
    float2 dummy;
    float4 edgeColor;
};

Texture2D texBase : register(t0);
SamplerState sampBase : register(s0);
Texture2D texMask : register(t1);
SamplerState sampMask : register(s1);

float4 main(PS_INPUT input) : SV_TARGET
{
    float4 color = texBase.Sample(sampBase, input.uv) * input.dif;

    if (color.a <= 0.01f && dot(color.rgb, float3(1.0f, 1.0f, 1.0f)) <= 0.01f)
    {
        return float4(0.0f, 0.0f, 0.0f, 0.0f);
    }

    float4 maskColor = texMask.Sample(sampMask, input.uv);
    float texMaskVal = (maskColor.a < 0.99f) ? maskColor.a : maskColor.r;

    // 1. 中心からの距離を計算（0.0が中心、1.0が外枠の円）
    float2 centered = input.uv - float2(0.5f, 0.5f);
    float dist = length(centered) * 2.0f;
    
    // 2. 四角形のフチ対策：円より外側（距離1.0以上）は完全に透明にして描画をスキップ
    if (dist > 1.0f)
    {
        return float4(0.0f, 0.0f, 0.0f, 0.0f);
    }

    // 3. ぐるぐる巻き（スパイラル）の計算を削除し、中心からの距離（放射状）に置き換え
    float radialVal = saturate(dist);

    // マスク画像が有効ならそれを使用、なければ距離ベースのグラデーションを使用
    float mask = (texMaskVal > 0.01f && texMaskVal < 0.99f) ? texMaskVal : radialVal;

    float width = (edgeWidth > 0.001f) ? edgeWidth : 0.08f;
    float currentProg = progress * (1.0f + width);

    // 進行度(progress)より外側は描画しない（これで中心から広がる動きになる）
    if (mask > currentProg)
    {
        return float4(0.0f, 0.0f, 0.0f, 0.0f);
    }

    // 境界線（エッジ）を光らせる処理
    float diff = currentProg - mask;
    if (diff >= 0.0f && diff < width)
    {
        float intensity = 1.0f - (diff / width);
        float3 glowCol = (dot(edgeColor.rgb, float3(1.0f, 1.0f, 1.0f)) > 0.01f)
                         ? edgeColor.rgb
                         : float3(0.4f, 0.8f, 1.0f);
        color.rgb += glowCol * intensity * 2.5f;
    }
    
    color.rgb *= color.a;
    
    return color;
}
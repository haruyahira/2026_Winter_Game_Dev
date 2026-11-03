struct PS_INPUT
{
    float4 pos : SV_POSITION;
    float4 dif : COLOR0;
    float4 spc : COLOR1;
    float2 uv : TEXCOORD0;
    float2 su : TEXCOORD1;
};

cbuffer cbParam : register(b3)
{
    float time;
    float beamSpeed;
    float2 dummy;
    float4 auraColor;
};

Texture2D texNoise : register(t1);
SamplerState sampBase : register(s1);

float4 main(PS_INPUT input) : SV_TARGET
{
    // 1. UVを高速スクロールさせる
    float2 scrollUV = input.uv;
    scrollUV.x -= time * beamSpeed;
    scrollUV.y -= time * (beamSpeed * 0.3f);

    // 2. ノイズ画像とサイン波を合成（画像が無くても絶対に黒くならずうねる）
    float texVal = texNoise.Sample(sampBase, scrollUV).r;
    float wave = sin(input.uv.x * 25.0f - time * 30.0f) * 0.5f + 0.5f;
    float noise = max(texVal, wave * 0.6f);

    // 3. C++側から渡された各層のカラーをベースにする
    float3 baseColor = (auraColor.r + auraColor.g + auraColor.b > 0.01f)
                       ? auraColor.rgb
                       : float3(0.3f, 0.6f, 1.0f);
    float baseAlpha = (auraColor.a > 0.01f) ? auraColor.a : 1.0f;

    // 4. ノイズが強い部分を白く輝かせ、エネルギーの奔流にする
    float coreMask = smoothstep(0.5f, 0.9f, noise);
    float3 finalColor = lerp(baseColor, float3(2.0f, 2.0f, 2.0f), coreMask * 0.6f);

    float intensity = lerp(0.8f, 1.5f, noise);
    finalColor *= intensity;

    float alpha = saturate(baseAlpha * intensity);

    return float4(finalColor * alpha, alpha);
}
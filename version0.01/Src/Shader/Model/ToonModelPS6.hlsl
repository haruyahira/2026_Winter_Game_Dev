// VS/PS共通
#include "../Common/VertexToPixelHeader.hlsli"

// IN
#define PS_INPUT VertexToPixelLitFog

// PS
#define TOON 1
#include "../Common/Pixel/PixelShader3DHeader.hlsli"

// 定数バッファ：スロット4番目
cbuffer cbParam : register(b4)
{
	float4 g_diff_color;
	float4 g_ambient_color;
	float3 g_light_dir;
	float  dummy;
}

float4 main(PS_INPUT PSInput) : SV_TARGET0
{

	float4 color;

	// テクスチャーの色を取得
	color = diffuseMapTexture.Sample(diffuseMapSampler, PSInput.uv);
	if (color.a < 0.01f)
	{
		discard;
	}
	
	// フォグ
	color.rgb *= PSInput.fogFactor;

	// ハーフランバート拡散照明によるディレクショナルライティング計算
	float p = dot(PSInput.normal * -1.0f, g_light_dir.xyz);
	p *= 0.4f;
	p = clamp(-1.0f, 1.4f, p);
	p = p * 0.5f + 0.5f;
	p = p * p;

	float4 toon = toonDiffuseGradTexture.Sample(toonDiffuseGradSampler, float2(p, 0.0f));

    float3 destColor = color.rgb * toon.rgb + g_ambient_color.rgb;
	
    return float4(destColor, color.a);

}

// VS/PS共通
#include "../Common/VertexToPixelHeader.hlsli"

// IN
#define PS_INPUT VertexToPixelLitFog

// PS
#include "../Common/Pixel/PixelShader3DHeader.hlsli"

// 定数バッファ：スロット4番目
cbuffer cbParam : register(b4)
{

	float4 g_diff_color;
	float4 g_ambient_color;
	float3 g_light_dir;
	float  dummy1;

	float  g_time;
	float  g_offset;
	float  dummy2;
	float  dummy3;

}

// HSVからRGBへの変換関数
float3 hsv2rgb(float h, float s, float v)
{
	return ((
		clamp(
			abs(
				frac(
					h + float3(0.0f, 2.0f, 1.0f) / 3.0f) * 6.0f - 3.0f) - 1.0f, 0.0f, 1.0f) - 1.0f) * s + 1.0f) * v;
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
	
	float lightDot = dot(PSInput.normal, -g_light_dir);
	float3 rgb = (color.rgb * (lightDot * g_diff_color.rgb)) + g_ambient_color.rgb;
	
	float3 pos = PSInput.worldPos;
	float time = g_time + g_offset;

	// 縦の大きさと時間で-1.0～1.0の値を作る
	float area = sin(pos.y * 0.008f - time * 1.0f);

	// 縦に特定範囲を作る(1 or 0)
	float isArea = step(0.996f, area * area);

	// 虹乗せ
	//-------------------------------------------------
	// 色相を計算（時間 + 縦方向の座標に基づいて変化）
	float hue = frac(pos.y * 0.05f);

	// HSVからRGBへの変換
	// 彩度1.0、輝度1.0で色を取得
	rgb += hsv2rgb(hue, 1.0f, 1.0f) * isArea;
	//-------------------------------------------------

	// フォグ
    rgb *= PSInput.fogFactor;
	
	return float4(rgb, color.a);

}

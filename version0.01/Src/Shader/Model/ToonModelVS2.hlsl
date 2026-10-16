// VS/PS共通
#include "../Common/VertexToPixelHeader.hlsli"

// IN
#include "../Common/Vertex/VertexInputType.hlsli"
#define VERTEX_INPUT DX_MV1_VERTEX_TYPE_8FRAME

// OUT
#define VS_OUTPUT VertexToPixelLitFog
#include "../Common/Vertex/VertexShader3DHeader.hlsli"

// 定数バッファ：スロット7番目
cbuffer cbParam : register(b7)
{
	float3 g_camera_pos;
	float dummy1;
	
	float g_fog_start;
	float g_fog_end;
	float dummy2;
	float dummy3;
}

#define L_W_MAT g_localWorldMatrix.lwMatrix

VS_OUTPUT main(VS_INPUT VSInput)
{

	// スキンメッシュ
	int4		lBoneIdx;
	float4		lL_W_Mat[3];

	// BONE4
	lBoneIdx = VSInput.blendIndices0;
	lL_W_Mat[0] = L_W_MAT[lBoneIdx.x + 0] * VSInput.blendWeight0.xxxx;
	lL_W_Mat[1] = L_W_MAT[lBoneIdx.x + 1] * VSInput.blendWeight0.xxxx;
	lL_W_Mat[2] = L_W_MAT[lBoneIdx.x + 2] * VSInput.blendWeight0.xxxx;

	lL_W_Mat[0] += L_W_MAT[lBoneIdx.y + 0] * VSInput.blendWeight0.yyyy;
	lL_W_Mat[1] += L_W_MAT[lBoneIdx.y + 1] * VSInput.blendWeight0.yyyy;
	lL_W_Mat[2] += L_W_MAT[lBoneIdx.y + 2] * VSInput.blendWeight0.yyyy;

	lL_W_Mat[0] += L_W_MAT[lBoneIdx.z + 0] * VSInput.blendWeight0.zzzz;
	lL_W_Mat[1] += L_W_MAT[lBoneIdx.z + 1] * VSInput.blendWeight0.zzzz;
	lL_W_Mat[2] += L_W_MAT[lBoneIdx.z + 2] * VSInput.blendWeight0.zzzz;

	lL_W_Mat[0] += L_W_MAT[lBoneIdx.w + 0] * VSInput.blendWeight0.wwww;
	lL_W_Mat[1] += L_W_MAT[lBoneIdx.w + 1] * VSInput.blendWeight0.wwww;
	lL_W_Mat[2] += L_W_MAT[lBoneIdx.w + 2] * VSInput.blendWeight0.wwww;

	// BONE8
	lBoneIdx = VSInput.blendIndices1;
	lL_W_Mat[0] += L_W_MAT[lBoneIdx.x + 0] * VSInput.blendWeight1.xxxx;
	lL_W_Mat[1] += L_W_MAT[lBoneIdx.x + 1] * VSInput.blendWeight1.xxxx;
	lL_W_Mat[2] += L_W_MAT[lBoneIdx.x + 2] * VSInput.blendWeight1.xxxx;

	lL_W_Mat[0] += L_W_MAT[lBoneIdx.y + 0] * VSInput.blendWeight1.yyyy;
	lL_W_Mat[1] += L_W_MAT[lBoneIdx.y + 1] * VSInput.blendWeight1.yyyy;
	lL_W_Mat[2] += L_W_MAT[lBoneIdx.y + 2] * VSInput.blendWeight1.yyyy;

	lL_W_Mat[0] += L_W_MAT[lBoneIdx.z + 0] * VSInput.blendWeight1.zzzz;
	lL_W_Mat[1] += L_W_MAT[lBoneIdx.z + 1] * VSInput.blendWeight1.zzzz;
	lL_W_Mat[2] += L_W_MAT[lBoneIdx.z + 2] * VSInput.blendWeight1.zzzz;

	lL_W_Mat[0] += L_W_MAT[lBoneIdx.w + 0] * VSInput.blendWeight1.wwww;
	lL_W_Mat[1] += L_W_MAT[lBoneIdx.w + 1] * VSInput.blendWeight1.wwww;
	lL_W_Mat[2] += L_W_MAT[lBoneIdx.w + 2] * VSInput.blendWeight1.wwww;


	VS_OUTPUT ret;

	// 頂点座標変換 +++++++++++++++++++++++++++++++++++++( 開始 )
	float4 lLocalPosition;
	float4 lWorldPosition;
	float4 lViewPosition;

	// float3 → float4
	lLocalPosition.xyz = VSInput.pos;
	lLocalPosition.w = 1.0f;

	// ローカル座標をワールド座標に変換(剛体)
	lWorldPosition.w = 1.0f;
	//lWorldPosition.xyz = mul(lLocalPosition, g_base.localWorldMatrix);
	lWorldPosition.x = dot(lLocalPosition, lL_W_Mat[0]);
	lWorldPosition.y = dot(lLocalPosition, lL_W_Mat[1]);
	lWorldPosition.z = dot(lLocalPosition, lL_W_Mat[2]);
	ret.worldPos = lWorldPosition.xyz;

	// ワールド座標をビュー座標に変換
	lViewPosition.w = 1.0f;
	lViewPosition.xyz = mul(lWorldPosition, g_base.viewMatrix);
	ret.vwPos = lViewPosition.xyz;

	// ビュー座標を射影座標に変換
	ret.svPos = mul(lViewPosition, g_base.projectionMatrix);

	// 頂点座標変換 +++++++++++++++++++++++++++++++++++++( 終了 )

	// その他、ピクセルシェーダへ引継&初期化 ++++++++++++( 開始 )
	// UV座標
	ret.uv.x = VSInput.uv0.x;
	ret.uv.y = VSInput.uv0.y;
	// 法線をローカル空間からワールド空間へ変換
	//ret.normal = VSInput.norm;
	//ret.normal = normalize(mul(VSInput.norm, (float3x3)g_base.localWorldMatrix));
	ret.normal.x = dot(VSInput.norm, lL_W_Mat[0].xyz);
	ret.normal.y = dot(VSInput.norm, lL_W_Mat[1].xyz);
	ret.normal.z = dot(VSInput.norm, lL_W_Mat[2].xyz);
	ret.normal = normalize(ret.normal);
	// ディフューズカラー
	ret.diffuse = VSInput.diffuse;
	// フォグの強さ(0.0:フォグが濃い、1.0:フォグが薄い)
	ret.fogFactor =
		clamp((g_fog_end - length(lWorldPosition.xyz - g_camera_pos)) / (g_fog_end - g_fog_start), 0.0, 1.0);
	// その他、ピクセルシェーダへ引継&初期化 ++++++++++++( 終了 )

	// 出力パラメータを返す
	return ret;

}
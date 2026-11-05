#pragma once
#include <DxLib.h>
#include <vector>

// 最大ライト数
#define MAX_POINT_LIGHT 32
#define MAX_SPOT_LIGHT  8

// ポイントライト (32バイト)
struct PointLightData
{
	VECTOR pos;       // 位置 (12バイト)
	float range;      // 影響範囲 (4バイト)
	COLOR_F color;    // 色 (16バイト)
};

// スポットライト (64バイト)
struct SpotLightData
{
	VECTOR pos;         // 位置 (12バイト)
	float range;        // 影響範囲 (4バイト)
	VECTOR direction;   // 照射方向 (12バイト)
	float innerConeCos; // 内側の光の角度(cos値) (4バイト)
	COLOR_F color;      // 色 (16バイト)
	float outerConeCos; // 外側の光の角度(cos値) (4バイト)
	float padding[3];   // 16バイト境界に合わせるためのパディング (12バイト)
};

// シェーダーに転送する定数バッファ全体 (16バイトの倍数)
struct LightEnvironmentBuffer
{
	PointLightData pointLights[MAX_POINT_LIGHT]; // ポイントライト配列
	int pointLightCount;                         // 現在のポイントライト数 (4バイト)
	float pad1[3];                               // パディング (12バイト)

	SpotLightData spotLights[MAX_SPOT_LIGHT];    // スポットライト配列
	int spotLightCount;                          // 現在のスポットライト数 (4バイト)
	float pad2[3];                               // パディング (12バイト)
};

class LightManager
{

public:

	LightManager();
	~LightManager();

	// 毎フレーム、またはシーン切り替え時にライトをリセットする
	void Clear();

	// ポイントライトを追加
	void AddPointLight(const VECTOR& pos, float range, const COLOR_F& color);

	// スポットライトを追加 (innerAngle, outerAngle は度数法 例: 30.0f)
	void AddSpotLight(const VECTOR& pos, const VECTOR& dir, float range, float innerAngle, float outerAngle, const COLOR_F& color);

	// CPUのデータをGPU(VRAM)の定数バッファに転送
	void UpdateBuffer();

	// 指定したスロット（レジスタ）に定数バッファをセットする
	void SetReservePS(int slotIndex);

private:

	int constBufHandle_;
	LightEnvironmentBuffer bufferData_;

};
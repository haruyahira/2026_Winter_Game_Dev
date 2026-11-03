#include "LightManager.h"
#include <math.h>

// 度数法をラジアンに変換するマクロ
#define DEG_TO_RAD(deg) ((deg) * 3.14159265358979323846f / 180.0f)

LightManager::LightManager()
{
	// 定数バッファの作成
	constBufHandle_ = CreateShaderConstantBuffer(sizeof(LightEnvironmentBuffer));
	Clear();
}

LightManager::~LightManager()
{
	DeleteShaderConstantBuffer(constBufHandle_);
}

void LightManager::Clear()
{
	bufferData_.pointLightCount = 0;
	bufferData_.spotLightCount = 0;
}

void LightManager::AddPointLight(const VECTOR& pos, float range, const COLOR_F& color)
{
	if (bufferData_.pointLightCount >= MAX_POINT_LIGHT) return;

	int idx = bufferData_.pointLightCount;
	bufferData_.pointLights[idx].pos = pos;
	bufferData_.pointLights[idx].range = range;
	bufferData_.pointLights[idx].color = color;

	bufferData_.pointLightCount++;
}

void LightManager::AddSpotLight(const VECTOR& pos, const VECTOR& dir, float range, float innerAngle, float outerAngle, const COLOR_F& color)
{
	if (bufferData_.spotLightCount >= MAX_SPOT_LIGHT) return;

	int idx = bufferData_.spotLightCount;
	bufferData_.spotLights[idx].pos = pos;
	bufferData_.spotLights[idx].range = range;

	// 方向ベクトルは正規化しておく
	VECTOR nDir = VNorm(dir);
	bufferData_.spotLights[idx].direction = nDir;

	// シェーダーでの計算を軽くするため、角度はあらかじめ Cosine に変換しておく
	bufferData_.spotLights[idx].innerConeCos = cosf(DEG_TO_RAD(innerAngle));
	bufferData_.spotLights[idx].outerConeCos = cosf(DEG_TO_RAD(outerAngle));
	bufferData_.spotLights[idx].color = color;

	bufferData_.spotLightCount++;
}

void LightManager::UpdateBuffer()
{
	// 1. DxLibが用意している定数バッファのメモリアドレスを取得
	void* pBuffer = GetBufferShaderConstantBuffer(constBufHandle_);

	if (pBuffer != nullptr)
	{
		// 2. 取得したアドレスに、構造体のデータを丸ごとコピーする
		memcpy(pBuffer, &bufferData_, sizeof(bufferData_));
	}

	// 3. GPU(VRAM)に転送して反映する（引数はハンドル1つだけ！）
	UpdateShaderConstantBuffer(constBufHandle_);
}

void LightManager::SetReservePS(int slotIndex)
{
	// ピクセルシェーダーの指定スロットにセット
	SetShaderConstantBuffer(constBufHandle_, DX_SHADERTYPE_PIXEL, slotIndex);
}
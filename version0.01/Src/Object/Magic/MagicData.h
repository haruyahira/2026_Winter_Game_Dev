#pragma once
#include <string>
#include <map>
#include <DxLib.h>
#include "../../Manager/ResourceManager.h"

// 魔法の設計図（パラメータ）
struct MagicData
{
	std::string name;            // 呪文の名前
	ResourceManager::SRC modelId;// 使用する3DモデルのID

	int damage;                  // 敵に与えるダメージ
	float speed;                 // 飛んでいく速さ
	float lifeTime;              // 寿命（秒）
	float collisionRadius;       // 当たり判定の大きさ
	float scale;                 // 見た目のモデルサイズ

	unsigned int expColorCore;   // 爆発エフェクトの芯の色
	unsigned int expColorAura;   // 爆発エフェクトのオーラの色

	float castTime;              // 詠唱にかかる時間（秒）
	unsigned int circleColor;    // 魔法陣の色 (GetColorで指定)

	float circleSize;            // 魔法陣の大きさ
	float circleDistance;        // 魔法陣のカメラからの距離

	float spawnDistance;         // 魔法の弾が発生するカメラからの距離
};

// 魔法データベース（辞書）
class MagicDatabase
{
public:
	static const std::map<std::string, MagicData>& GetAll()
	{
		static std::map<std::string, MagicData> data = {
			// fireball: 大きさ 40.0f, 魔法陣距離 150.0f, 弾の距離 180.0f
				{ "fireball", MagicData{ "fire ball",ResourceManager::SRC::FIREBALL,
					20, 10.0f, 5.0f, 25.0f, 1.0f,
					GetColor(255, 255, 180), GetColor(255, 100, 0),
					2.0f, GetColor(255, 50, 0),
					40.0f, 150.0f,
					180.0f } },

				// waterball
				{ "waterball", MagicData{ "water ball",ResourceManager::SRC::FIREBALL,
					12, 8.0f, 4.0f, 30.0f, 1.2f,
					GetColor(180, 240, 255), GetColor(0, 100, 255),
					1.5f, GetColor(50, 200, 255),
					70.0f, 150.0f,
					180.0f } },

				// thunderball
				{ "thunderball", MagicData{ "thunder ball",ResourceManager::SRC::FIREBALL,
					35, 22.0f, 2.0f, 15.0f, 0.7f, GetColor(255, 255, 255), GetColor(200, 200, 0),
					3.0f, GetColor(255, 230, 50),
					70.0f, 150.0f,
					180.0f } },

				// woodball
				{ "woodball", MagicData{ "wood ball",ResourceManager::SRC::FIREBALL,
					10, 6.0f, 7.0f, 40.0f, 1.5f,
					GetColor(150, 255, 150), GetColor(0, 180, 0),
					1.0f, GetColor(50, 255, 50),
					70.0f, 150.0f,
					180.0f } }, 

				// zoltraak: 魔法陣距離 400.0f, 弾の距離 400.0f (同じ位置から発射)
				{ "zoltraak", MagicData{ "zoltraak", ResourceManager::SRC::ZOLTRAAK,
					10, 0.0f, 9999.0f, 0.0f, 2.5f,
					GetColor(255, 255, 255), GetColor(50, 100, 255),
					0.5f, GetColor(175, 215, 255),
					400.0f, 610.0f,
					420.0f } 
				}
		};
		return data;
	}

	// 詠唱された文字が魔法として存在するかチェック
	static bool IsValidSpell(const std::string& spell)
	{
		return GetAll().count(spell) > 0;
	}

	// 呪文のデータを取り出す
	static MagicData GetData(const std::string& spell)
	{
		return GetAll().at(spell);
	}
};
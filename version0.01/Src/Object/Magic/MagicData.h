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
};

// 魔法データベース（辞書）
class MagicDatabase
{
public:
	static const std::map<std::string, MagicData>& GetAll()
	{
		// ★修正：検索キー（左側の文字列）からスペースを削除しました！
		// プレイヤーの入力もスペースが削除されるので、これで完全一致します
		static std::map<std::string, MagicData> data = {
			{ "fireball",    { "fire ball",    ResourceManager::SRC::FIREBALL, 20, 10.0f, 5.0f, 25.0f, 1.0f, GetColor(255, 255, 180), GetColor(255, 100, 0)   } },
			{ "waterball",   { "water ball",   ResourceManager::SRC::FIREBALL, 12, 8.0f, 4.0f, 30.0f, 1.2f, GetColor(180, 240, 255), GetColor(0, 100, 255)   } },
			{ "thunderball", { "thunder ball", ResourceManager::SRC::FIREBALL, 35, 22.0f, 2.0f, 15.0f, 0.7f, GetColor(255, 255, 255), GetColor(200, 200, 0)   } },
			{ "woodball",    { "wood ball",    ResourceManager::SRC::FIREBALL, 10, 6.0f, 7.0f, 40.0f, 1.5f, GetColor(150, 255, 150), GetColor(0, 180, 0)     } }
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
#pragma once
#include <string>
#include <DxLib.h>
#include "../../Manager/SceneManager.h"

class SystemMagic
{
public:
	// システム魔法（シーン遷移や終了）なら実行して true を返す
	static bool Execute(const std::string& spell)
	{
		// 終了魔法
		if (spell == "exit" || spell == "quit")
		{
			PostMessage(GetMainWindowHandle(), WM_CLOSE, 0, 0);
			return true;
		}
		// 開始・リトライ魔法
		if (spell == "gamestart" || spell == "retry")
		{
			SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::GAME);
			return true;
		}
		// タイトル帰還魔法
		if (spell == "totitle" || spell == "title")
		{
			SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::TITLE);
			return true;
		}

		return false; // システム魔法ではない
	}
};
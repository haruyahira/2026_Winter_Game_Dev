#pragma once

class FpsController
{
private:
	int startTime_;
	int count_;
	float fps_;
	static const int UPDATE_FRAME = 60; // 60フレーム（約1秒）ごとに計算を更新

public:
	FpsController();
	void Update();
	void Draw();
};
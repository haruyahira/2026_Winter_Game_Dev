#pragma once

class FpsController
{
public:

	FpsController();
	void Update();
	void Draw();

private:

	static const int UPDATE_FRAME = 60; // 60フレーム（約1秒）ごとに計算を更新
	
	int startTime_;
	int count_;
	float fps_;

};
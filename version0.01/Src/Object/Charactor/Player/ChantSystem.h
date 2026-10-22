// Src/Object/Charactor/Player/ChantSystem.h
#pragma once
#include <string>

class ChantSystem
{
public:
	ChantSystem(void);
	~ChantSystem(void);

	// 詠唱の開始と強制キャンセル
	void Start(void);
	void Cancel(void);

	// 毎フレームのタイピング判定
	void Update(void);

	// 画面下部に横幅いっぱいで打ち込んだ文字を表示する
	void Draw(void);

	// 状態の確認
	bool IsChanting(void) const { return isChanting_; }
	bool IsFinished(void) const { return isFinished_; }

	// 打ち込んだ呪文（文字列）を取得
	std::string GetTypedText(void) const { return typedText_; }

	// ★ シーンに合わせてヒントテキストを変更する
	void SetHintText(const std::string& hint) { hintText_ = hint; }

private:
	bool isChanting_;
	bool isFinished_;
	std::string typedText_;
	std::string hintText_;

	// 画面解像度に合わせて自動生成するフォントハンドル
	int fontMain_;
	int fontSub_;
	int lastScreenHeight_;

	void UpdateFonts(int screenH);
};
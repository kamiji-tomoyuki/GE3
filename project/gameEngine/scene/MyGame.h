#pragma once
#include "Framework.h"
#include "TitleScene.h"
#include "GamePlayScene.h"

// ゲーム全体
class MyGame : public Framework
{
public:
	// 初期化
	void Initialize() override;

	// 終了
	void Finalize() override;

	// 更新処理
	void Update() override;

	// 描画処理
	void Draw() override;

private: // メンバ変数
	// タイトルシーン
	TitleScene* titleScene_ = nullptr;

	// ゲームシーン
	GamePlayScene* gamePlayScene_ = nullptr;

};


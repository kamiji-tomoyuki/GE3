#pragma once
#include "Framework.h"

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
};


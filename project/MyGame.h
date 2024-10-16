#pragma once
#include <Audio.h>
#include <DirectXCommon.h>
#include <Input.h>


// ゲーム全体
class MyGame
{
public:
	// 初期化
	void Initialize();

	// 終了
	void Finalize();

	// 更新処理
	void Update();

	// 描画処理
	void Draw();

private: // メンバ変数
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;
};


#pragma once
#include <Audio.h>
#include <DirectXCommon.h>
#include <ImGuiManager.h>
#include <Input.h>
#include <ModelManager.h>
#include <Object3dCommon.h>
#include <SpriteCommon.h>
#include <SrvManager.h>
#include <TextureManager.h>
#include <WinApp.h>

// フレームワーク
class Framework
{
public:
	virtual ~Framework() = default;

	// 実行
	void Run();


	// 初期化
	virtual void Initialize();

	// 終了
	virtual void Finalize();

	// 更新処理
	virtual void Update();

	// 描画処理
	virtual void Draw() = 0;

	// 終了リクエストの取得
	virtual bool IsEndRequest() { return endRequest; }

private:
	bool endRequest = false;	// 終了フラグ

protected:
	// 汎用性の高いシステム
	WinApp* winApp = nullptr;				// WindowsAPI
	DirectXCommon* dxCommon = nullptr;		// DirectX
	Input* input = nullptr;					// キーボード入力
	SrvManager* srvManager = nullptr;		// SRVマネージャ
	Audio* audio = nullptr;					// オーディオ
	ImGuiManager* imGuiManager = nullptr;	// ImGuiマネージャ

	SpriteCommon* spriteCommon = nullptr;		// スプライト
	Object3dCommon* object3dCommon = nullptr;	// 3Dオブジェクト

};


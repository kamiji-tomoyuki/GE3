#pragma once
#include "Windows.h"
#include "Audio.h"
#include "Camera.h"
#include "CameraManager.h"
#include "DirectXCommon.h"
#include "D3DResourceLeakChecker.h"
#include "Input.h"
#include "Model.h"
#include "ModelCommon.h"
#include "ModelManager.h"
#include "Object3d.h"
#include "Object3dCommon.h"
#include "Sprite.h"
#include "SpriteCommon.h"
#include "TextureManager.h"
#include "WinApp.h"

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

	// 終了リクエストの取得
	bool IsEndRequest() { return endRequest; }

private: // メンバ変数
#pragma region 基礎システムの初期化

	WinApp* winApp = nullptr;			// WindowsAPI
	DirectXCommon* dxCommon = nullptr;	// DirectX
	Input* input = nullptr;				// キーボード入力
	Audio* audio = nullptr;				// オーディオ

	SpriteCommon* spriteCommon = nullptr;		// スプライト
	Object3dCommon* object3dCommon = nullptr;	// 3Dオブジェクト

	bool endRequest = false;				// 終了フラグ

#pragma endregion 基礎システムの初期化
	
	Camera* camera = nullptr;	// カメラ

	std::vector<Sprite*> sprites;
	uint32_t spriteNum = 1;		/* スプライト枚数 */

	std::vector<Object3d*> object3ds;
	uint32_t objectNum = 2;		/* オブジェクト個数 */

	SoundData soundData;		// オーディオ読み込み

};


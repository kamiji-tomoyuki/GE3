#pragma once
#include "Windows.h"
#include "Audio.h"
#include "Camera.h"
#include "CameraManager.h"
#include "D3DResourceLeakChecker.h"
#include "Model.h"
#include "ModelCommon.h"
#include "ModelManager.h"
#include "Object3d.h"
#include "Object3dCommon.h"
#include "Sprite.h"
#include "TextureManager.h"

class GamePlayScene
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
	// カメラ
	Camera* camera = nullptr;	
	// サウンド
	SoundData soundData;

	// 2Dスプライト
	std::vector<Sprite*> sprites;
	// 3Dオブジェクト
	std::vector<Object3d*> object3ds;

};


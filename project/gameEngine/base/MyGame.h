#pragma once
#include "Windows.h"
#include "Camera.h"
#include "CameraManager.h"
#include "D3DResourceLeakChecker.h"
#include "Model.h"
#include "ModelCommon.h"
#include "ModelManager.h"
#include "Object3d.h"
#include "Sprite.h"
#include "TextureManager.h"

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
	Camera* camera = nullptr;	// カメラ

	std::vector<Sprite*> sprites;
	uint32_t spriteNum = 1;		/* スプライト枚数 */

	std::vector<Object3d*> object3ds;
	uint32_t objectNum = 2;		/* オブジェクト個数 */

	SoundData soundData;		// オーディオ読み込み

};


#include "Windows.h"

#include <dxgidebug.h>

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
#include "imgui_impl_win32.h"
#include "imgui_impl_dx12.h"

#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dxcompiler.lib")

///////////////////////////////////////////////////////////

// Windowsアプリでのエントリーポイント
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	D3DResourceLeakChecker leakCheck;

	//=========================================================
	WinApp* winApp = nullptr;//WinApp
	winApp = new WinApp();
	winApp->Initialize();

	Input* input = nullptr;//input
	input = new Input();
	input->Initialize(winApp);

	DirectXCommon* dxCommon = nullptr;//dxCommon
	dxCommon = new DirectXCommon();
	dxCommon->Initialize(winApp);

#pragma region 基礎システムの初期化

	TextureManager::GetInstance()->Initialize(dxCommon);

	SpriteCommon* spriteCommon = nullptr;
	spriteCommon = new SpriteCommon();
	spriteCommon->Initialize(dxCommon);

	Object3dCommon* object3dCommon = nullptr;
	object3dCommon = new Object3dCommon();
	object3dCommon->Initialize(dxCommon);

	ModelManager::GetInstance()->Initialize(dxCommon);
	ModelManager::GetInstance()->LoadModel("plane.obj");
	ModelManager::GetInstance()->LoadModel("axis.obj");

	Camera* camera = new Camera();
	camera->SetRotate({ 0.3f,0.0f,0.0f });
	camera->SetTranslate({ 0.0f,4.0f,-10.0f });
	object3dCommon->SetDefaultCamera(camera);
	
#pragma endregion 基礎システムの初期化

#pragma region 初期化

	// --- Sprite ---
	std::vector<Sprite*> sprites;
	uint32_t spriteNum = 1;

	for (uint32_t i = 0; i < spriteNum; ++i) {
		Sprite* sprite = new Sprite();
		std::string textureFile = { "resources/images/uvChecker.png" };
		if (i % 2 == 1) {
			textureFile = { "resources/images/monsterBall.png" };
		}
		sprite->Initialize(spriteCommon, textureFile);
		sprites.push_back(sprite);
	}

	// --- Model ---
	std::vector<Object3d*> object3ds;
	uint32_t objectNum = 2;

	for (uint32_t i = 0; i < objectNum; ++i) {
		Object3d* object = new Object3d();
		object->Initialize(object3dCommon);

		Vector3 position;
		position.x = i * 2.0f;

		object->SetPosition(position);

		if (i == 0) {
			object->SetModel("plane.obj");
		}
		if (i == 1) {
			object->SetModel("axis.obj");
		}

		object3ds.push_back(object);
	}

#pragma endregion 初期化

	//=========================================================

	// ウィンドウのxボタンが押されるまでループ
	while (true) { // ゲームループ！！！！！！！！！！
		// Windowにメッセージがきてたら最優先で処理させる
		if (winApp->ProcessMessage()) {
			//ゲームループを抜ける
			break;
		}
		// ゲームの処理
		//=========================================================
		//入力の更新
		input->Update();

		//カメラの更新
		camera->Update();

#pragma region スプライト

		for (uint32_t i = 0; i < spriteNum; ++i) {
			sprites[i]->Update();
			
			Vector2 position = { 200.0f * i, 0.0f };
			sprites[i]->SetPosition(position);

			float rotation = sprites[i]->GetRotate();
			sprites[i]->SetRotate(rotation);

			Vector2 size = {200.0f,200.0f};
			sprites[i]->SetSize(size);

			Vector4 color = sprites[i]->GetColor();
			sprites[i]->SetColor(color);
		}

#pragma endregion スプライト

#pragma region 3Dオブジェクト

		for (uint32_t i = 0; i < objectNum; ++i) {
			Object3d* obj = object3ds[i];
			obj->Update();

			Vector3 rotate = obj->GetRotate();
			if (i == 0) {
				rotate.x += 0.01f;
			}
			else if (i == 1) {
				rotate.y += 0.01f;
			}
			
			obj->SetRotate(rotate);
		}

#pragma endregion 3Dオブジェクト

		//描画前処理(DirectX)
		dxCommon->PreDraw();

		// 描画前処理(Object)
		object3dCommon->PreDraw();

		//描画前処理(Sprite)
		spriteCommon->PreDraw();

		for (uint32_t i = 0; i < spriteNum; ++i) {
			sprites[i]->Draw();
		}

		for (auto& obj : object3ds) {
			obj->Draw();
		}

		//描画後処理
		dxCommon->PostDraw();

		//========================================================= 

#ifdef _DEBUG


#endif

	}

	//=================================================
#pragma region 解放処理
	delete input;

	winApp->Finalize();
	delete winApp;
	winApp = nullptr;

	delete dxCommon;

	delete spriteCommon;
	delete object3dCommon;

	delete camera;

	TextureManager::GetInstance()->Finalize();
	for (uint32_t i = 0; i < spriteNum; ++i) {
		delete sprites[i];
	}

	ModelManager::GetInstance()->Finalize();
	for (auto& obj : object3ds) {
		delete obj;
	}

#pragma endregion 解放処理
	//=================================================

	return 0;

}
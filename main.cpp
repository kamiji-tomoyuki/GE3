#include "Windows.h"

#include <dxgidebug.h>

#include "Input/WinApp.h"
#include "Input/Input.h"
#include "Input/DirectXCommon.h"
#include "Input/D3DResourceLeakChecker.h"
#include "Input/Sprite.h"
#include "Input/SpriteCommon.h"
#include "Input/TextureManager.h"
#include "externals/imgui/imgui_impl_win32.h"
#include "externals/imgui/imgui_impl_dx12.h"

#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dxcompiler.lib")

///////////////////////////////////////////////////////////

// Windowsアプリでのエントリーポイント
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	D3DResourceLeakChecker leakCheck;

	//=========================================================
#pragma region 初期化
	WinApp* winApp = nullptr;//WinApp
	winApp = new WinApp();
	winApp->Initialize();

	Input* input = nullptr;//input
	input = new Input();
	input->Initialize(winApp);

	DirectXCommon* dxCommon = nullptr;//dxCommon
	dxCommon = new DirectXCommon();
	dxCommon->Initialize(winApp);

	TextureManager::GetInstance()->Initialize(dxCommon);


	SpriteCommon* spriteCommon = nullptr;//スプライト共通部
	spriteCommon = new SpriteCommon();
	spriteCommon->Initialize(dxCommon);

	std::vector<Sprite*> sprites;
	uint32_t spriteNum = 1;

	std::string textureFiles[] = { "resources/images/monsterBall.png" ,"resources/images/uvChecker.png" };
	for (uint32_t i = 0; i < spriteNum; ++i) {
		Sprite* sprite = new Sprite();
		std::string textureFile = { "resources/images/uvChecker.png" };
		if (i % 2 == 1) {
			textureFile = { "resources/images/monsterBall.png" };
		}
		sprite->Initialize(spriteCommon, textureFile);
		sprites.push_back(sprite);
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

#pragma region スプライト

		for (uint32_t i = 0; i < spriteNum; ++i) {
			Vector2 position = { 200.0f * i, 0.0f };
			sprites[i]->SetPosition(position);

			float rotation = sprites[i]->GetRotate();
			sprites[i]->SetRotate(rotation);

			Vector2 size = {100.0f,100.0f};
			//sprites[i]->SetSize(size);

			Vector4 color = sprites[i]->GetColor();
			sprites[i]->SetColor(color);
		}

#pragma endregion スプライト

		//描画前処理(DirectX)
		dxCommon->PreDraw();

		//描画前処理(Sprite)
		spriteCommon->PreDraw();

		for (uint32_t i = 0; i < spriteNum; ++i) {
			sprites[i]->Update();
			sprites[i]->Draw();
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

	TextureManager::GetInstance()->Finalize();

	delete dxCommon;

	delete spriteCommon;
	for (uint32_t i = 0; i < spriteNum; ++i) {
		delete sprites[i];
	}
#pragma endregion 解放処理
	//=================================================

	return 0;

}
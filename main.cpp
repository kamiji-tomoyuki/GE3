#include "Windows.h"

#include <dxgidebug.h>

#include "Input/WinApp.h"
#include "Input/Input.h"
#include "Input/DirectXCommon.h"
#include "Input/D3DResourceLeakChecker.h"

#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dxcompiler.lib")

///////////////////////////////////////////////////////////

// Windowsアプリでのエントリーポイント
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	D3DResourceLeakChecker leakCheck;

	//=========================================================
	//取得
	WinApp* winApp = nullptr;//WinApp
	winApp = new WinApp();
	winApp->Initialize();

	Input* input = nullptr;//input
	input = new Input();
	input->Initialize(winApp);

	DirectXCommon* dxCommon = nullptr;//dxCommon
	dxCommon = new DirectXCommon();
	dxCommon->Initialize(winApp);

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

		//描画前処理
		dxCommon->PreDraw();
		
		//


		//描画後処理
		dxCommon->PostDraw();

		//========================================================= 

	}

	//=================================================
	delete input;

	winApp->Finalize();
	delete winApp;
	winApp = nullptr;

	delete dxCommon;
	//=================================================

	return 0;

}
#include "MyGame.h"

#include "imgui_impl_win32.h"
#include "imgui_impl_dx12.h"

#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dxcompiler.lib")

///////////////////////////////////////////////////////////

// Windowsアプリでのエントリーポイント
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	MyGame game;
	game.Initialize();
	
	// ウィンドウのxボタンが押されるまでループ
	while (true) { 
		// 終了リクエストが来たら抜ける
		if (game.IsEndRequest()) {
			break;
		}
		// ===== 更新処理 =====
		
		// 毎フレーム更新
		game.Update();


		// ===== 描画処理 =====

		// 描画
		game.Draw();

	}

	// ゲーム終了
	game.Finalize();
	
	return 0;

}
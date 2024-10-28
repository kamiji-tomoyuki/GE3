#include "Framework.h"
#include <imgui.h>

void Framework::Run()
{
	Initialize();

	while (true) {
		// 終了リクエストが来たら抜ける
		if (IsEndRequest()) {
			break;
		}
		// ===== 更新処理 =====

		// ImGui開始
		imGuiManager->Begin();

		// 毎フレーム更新
		Update();

		// ImGui終了
		imGuiManager->End();

		// ===== 描画処理 =====

		// 描画
		Draw();

	}

	// ゲーム終了
	Finalize();
}

void Framework::Initialize()
{
	// WindowsAPI
	winApp = new WinApp();
	winApp->Initialize();

	// DirectX
	dxCommon = new DirectXCommon();
	dxCommon->Initialize(winApp);

	// キーボード入力
	input = new Input();
	input->Initialize(winApp);

	// SRVマネージャ
	srvManager = new SrvManager();
	srvManager->Initialize(dxCommon);

	// オーディオ
	audio = new Audio();
	audio->Initialize();

	// ImGui
	imGuiManager = new ImGuiManager();
	imGuiManager->Initialize(winApp, dxCommon);

	// スプライト
	TextureManager::GetInstance()->Initialize(dxCommon, srvManager);
	spriteCommon = new SpriteCommon();
	spriteCommon->Initialize(dxCommon);

	// 3Dオブジェクト
	ModelManager::GetInstance()->Initialize(dxCommon);
	object3dCommon = new Object3dCommon();
	object3dCommon->Initialize(dxCommon);
}

void Framework::Update()
{
	// 入力の更新
	input->Update();
}

void Framework::Finalize()
{
	winApp->Finalize();
	delete winApp;
	winApp = nullptr;

	delete dxCommon;
	delete input;
	delete srvManager;
	delete audio;

	delete spriteCommon;
	delete object3dCommon;

	imGuiManager->Finalize();
	delete imGuiManager;
}


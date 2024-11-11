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
	input = Input::GetInstance();
	input->Initialize(winApp);

	// SRVマネージャ
	srvManager = new SrvManager();
	srvManager->Initialize(dxCommon);

	// オーディオ
	audio = Audio::GetInstance();
	audio->Initialize();

	// ImGui
	imGuiManager = new ImGuiManager();
	imGuiManager->Initialize(winApp, dxCommon);

	// シーンマネージャ
	sceneManager_ = SceneManager::GetInstance();

	// スプライト
	spriteCommon = SpriteCommon::GetInstance();
	spriteCommon->Initialize(dxCommon);

	// テクスチャマネージャ
	textureManager = TextureManager::GetInstance();
	textureManager->Initialize(dxCommon, srvManager);

	// 3Dオブジェクト
	object3dCommon = Object3dCommon::GetInstance();
	object3dCommon->Initialize(dxCommon);

	// モデルマネージャ
	modelManager = ModelManager::GetInstance();
	modelManager->Initialize(dxCommon);

}

void Framework::Update()
{
	// シーンマネージャの更新
	sceneManager_->Update();

	// 入力の更新
	input->Update();
}

void Framework::Finalize()
{
	winApp->Finalize();
	delete winApp;
	winApp = nullptr;

	delete dxCommon;
	input->Finalize();
	delete srvManager;

	delete sceneFactory_;
	sceneManager_->Finalize();

	audio->Finalize();
	spriteCommon->Finalize();
	textureManager->Finalize();
	object3dCommon->Finalize();
	modelManager->Finalize();

	imGuiManager->Finalize();
	delete imGuiManager;
}


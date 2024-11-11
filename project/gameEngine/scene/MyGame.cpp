#include "MyGame.h"

void MyGame::Initialize()
{
	// --- 基底クラスの初期化 ---
	Framework::Initialize();

	// --- 各シーンの初期化 ---
	titleScene_ = new TitleScene();
	titleScene_->Initialize();

	//gamePlayScene_ = new GamePlayScene();
	//gamePlayScene_->Initialize();
	
}

void MyGame::Finalize()
{
	// --- リークチェック ---
	D3DResourceLeakChecker leakCheck;

	// --- 解放処理 ---
	titleScene_->Finalize();
	delete titleScene_;

	//gamePlayScene_->Finalize();
	//delete gamePlayScene_;

	Framework::Finalize();	// 基底クラスの解放処理
	
}

void MyGame::Update()
{
	// --- 基底クラスの更新処理 ---
	Framework::Update();

	// --- 各シーンの更新 ---
	titleScene_->Update();
	//gamePlayScene_->Update();

}

void MyGame::Draw()
{
	//描画前処理(SRV)
	srvManager->PreDraw();

	//描画前処理(DirectX)
	dxCommon->PreDraw();

	// --- 各シーンの描画処理 ---

	titleScene_->Draw();
	//gamePlayScene_->Draw();



	// -----------------------

	imGuiManager->Draw();

	// 描画後処理
	dxCommon->PostDraw();
}

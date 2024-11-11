#include "MyGame.h"

#include "SceneFactory.h"

void MyGame::Initialize()
{
	// --- 基底クラスの初期化 ---
	Framework::Initialize();

	// --- シーンの生成・初期化 ---
	// シーンマネージャに最初のシーンをセット	
	sceneFactory_ = new SceneFactory();
	SceneManager::GetInstance()->SetSceneFactory(sceneFactory_);

	// 最初のシーンを設定
	SceneManager::GetInstance()->ChangeScene("TITLE");
	
}

void MyGame::Finalize()
{
	// --- リークチェック ---
	D3DResourceLeakChecker leakCheck;

	Framework::Finalize();	// 基底クラスの解放処理
	
}

void MyGame::Update()
{
	// --- 基底クラスの更新処理 ---
	Framework::Update();

}

void MyGame::Draw()
{
	//描画前処理(SRV)
	srvManager->PreDraw();

	//描画前処理(DirectX)
	dxCommon->PreDraw();

	// --- シーンの描画処理 ---

	sceneManager_->Draw();

	// -----------------------

	imGuiManager->Draw();

	// 描画後処理
	dxCommon->PostDraw();
}

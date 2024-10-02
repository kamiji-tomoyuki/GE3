#include "Windows.h"

#include <dxgidebug.h>

#include "Input/WinApp.h"
#include "Input/Input.h"
#include "Input/DirectXCommon.h"
#include "Input/D3DResourceLeakChecker.h"
#include "Input/Sprite.h"
#include "Input/SpriteCommon.h"

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

	SpriteCommon* spriteCommon = nullptr;//スプライト共通部
	spriteCommon = new SpriteCommon();
	spriteCommon->Initialize(dxCommon);

	Sprite* sprite = new Sprite();
	sprite->Initialize(spriteCommon);

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
		// Textureを読んで転送する
		DirectX::ScratchImage mipImages = dxCommon->LoadTexture("./resources/images/uvChecker.png");
		const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
		Microsoft::WRL::ComPtr<ID3D12Resource> textureResource = dxCommon->CreateTextureResources(metadata);
		/*エラー原因 ->*/Microsoft::WRL::ComPtr<ID3D12Resource> interminalResource = dxCommon->UploadTextureData(textureResource, mipImages);

		// metaDataを基にSRVの設定
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
		srvDesc.Format = metadata.format;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = UINT(metadata.mipLevels);

		// SRVを作成するDescriptorHeapの場所を決める
		D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU = dxCommon->GetSRVCPUDescriptorHandle(2);
		D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU = dxCommon->GetSRVGPUDescriptorHandle(2);

		dxCommon->GetDevice()->CreateShaderResourceView(textureResource.Get(), &srvDesc, textureSrvHandleCPU);
#pragma endregion スプライト

		//描画前処理(DirectX)
		dxCommon->PreDraw();

		//描画前処理(Sprite)
		spriteCommon->PreDraw();

		sprite->Update();
		sprite->Draw();

		//描画後処理
		dxCommon->PostDraw();

		//========================================================= 

	}

	//=================================================
#pragma region 解放処理
	delete input;

	winApp->Finalize();
	delete winApp;
	winApp = nullptr;

	delete dxCommon;

	delete spriteCommon;
	delete sprite;
#pragma endregion 解放処理
	//=================================================

	return 0;

}
#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>

#include "WinApp.h"

class DirectXCommon
{
public://メンバ関数
	//初期化
	void Initialize(WinApp* winApp);
	//更新
	void Update();

	//初期化・生成
	void DviceInitialize();				//デバイス
	void CommandInitialize();			//コマンド関連
	void SwapChainCreate();				//スワップチェーン
	void DepthBufferCreate();			//深度バッファ
	void DescriptorHeapCreate();		//デスクリプタヒープ
	void RenderTargetViewInitialize();	//レンダーターゲットビュー
	void DepthStencilViewInitialize();	//深度ステンシルビュー
	void FenceInitialize();				//フェンス
	void ViewportRectInitialize();		//ビューポート矩形
	void ScissorRect();					//シザリング矩形
	void DXCCompilerCreate();			//DXCコンパイラ
	void ImGuiInitialize();				//ImGui

private:
	//DirectX12デバイス
	Microsoft::WRL::ComPtr<ID3D12Device> device_;
	//DXGIファクトリ
	Microsoft::WRL::ComPtr<IDXGIFactory7> dxgiFactory_;

	//WindowsAPI
	WinApp* winApp_ = nullptr;

};


#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>

#include "WinApp.h"
#include "Function/Logger.h"
#include "Function/StringUtility.h"

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

	//DXGIファクトリー
	Microsoft::WRL::ComPtr<IDXGIFactory7> dxgiFactory = nullptr;

	//コマンド関連の変数
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator = nullptr;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList = nullptr;
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue = nullptr;

	//スワップチェーン
	Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain = nullptr;


};


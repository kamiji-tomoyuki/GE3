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

	//DescriptorHeapの生成
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap(Microsoft::WRL::ComPtr<ID3D12Device> device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible);

	//DescriptorHandleの取得
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap, uint32_t descriptorSize, uint32_t index);
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap, uint32_t descriptorSize, uint32_t index);
	//DescriptorHandleの取得(SRV)
	D3D12_CPU_DESCRIPTOR_HANDLE GetSRVCPUDescriptorHandle(uint32_t index);
	D3D12_GPU_DESCRIPTOR_HANDLE GetSRVGPUDescriptorHandle(uint32_t index);

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

	//DescriptorSize
	uint32_t descriptorSizeRTV;
	uint32_t descriptorSizeSRV;
	uint32_t descriptorSizeDSV;
	//DescriptorHeap
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvDescriptorHeap;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvDescriptorHeap;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvDescriptorHeap;

	//swapChainResources
	std::array<Microsoft::WRL::ComPtr<ID3D12Resource>,2> swapChainResources;
	// ディスクリプタ * 2
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[2];

};


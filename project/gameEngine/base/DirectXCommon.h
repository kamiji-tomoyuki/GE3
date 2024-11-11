#pragma once
#include <array>
#include <chrono>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <dxcapi.h>

#include "WinApp.h"

#include "Logger.h"
#include "StringUtility.h"

#include "../../externals/DirectXTex/DirectXTex.h"

class DirectXCommon
{
public://メンバ関数
	// 初期化
	void Initialize(WinApp* winApp);
	// 更新
	void Update();

	// 初期化・生成
	void DviceInitialize();				// デバイス
	void CommandInitialize();			// コマンド関連
	void SwapChainCreate();				// スワップチェーン
	void DepthBufferCreate();			// 深度バッファ
	void DescriptorHeapCreate();		// デスクリプタヒープ
	void RenderTargetViewInitialize();	// レンダーターゲットビュー
	void DepthStencilViewInitialize();	// 深度ステンシルビュー
	void FenceInitialize();				// フェンス
	void ViewportRectInitialize();		// ビューポート矩形
	void ScissorRect();					// シザリング矩形
	void DXCCompilerCreate();			// DXCコンパイラ

	// 描画処理
	void PreDraw();	// 前
	void PostDraw();// 後



public:
	// DescriptorHeapの生成
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap(Microsoft::WRL::ComPtr<ID3D12Device> device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible);

	// DescriptorHandleの取得
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap, uint32_t descriptorSize, uint32_t index);
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap, uint32_t descriptorSize, uint32_t index);
	// DescriptorHandleの取得(SRV)
	D3D12_CPU_DESCRIPTOR_HANDLE GetSRVCPUDescriptorHandle(uint32_t index);
	D3D12_GPU_DESCRIPTOR_HANDLE GetSRVGPUDescriptorHandle(uint32_t index);

	// DepthStencilTextureResourceの生成
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateDepthStencilTextureResource(Microsoft::WRL::ComPtr<ID3D12Device> device, int32_t width, int32_t height);

	// シェーダーのコンパイル
	IDxcBlob* CompileShader(const std::wstring& filePath, const wchar_t* profile);

	// リソース生成関数
	// 最大SRV数(最大テクスチャ枚数)
	static const uint32_t kMaxSRVCount;
	// バッファリソースの生成
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(size_t sizeInBytes);
	// テクスチャリソースの生成
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateTextureResources(const DirectX::TexMetadata& metadata);
	// テクスチャデータの転送
	[[nodiscard]]
	Microsoft::WRL::ComPtr<ID3D12Resource> UploadTextureData(Microsoft::WRL::ComPtr<ID3D12Resource> texture, const DirectX::ScratchImage& mipImages);

	// テクスチャファイルの読み込み
	static DirectX::ScratchImage LoadTexture(const std::string& filePath);

	// deviceを取得
	Microsoft::WRL::ComPtr<ID3D12Device> GetDevice()const { return device_; }
	// commandListを取得
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>GetCommandList()const { return commandList; }

	// swapChainDescを取得
	DXGI_SWAP_CHAIN_DESC1 GetSwapChainDesc() { return swapChainDesc; }
	// rtvDescを取得
	D3D12_RENDER_TARGET_VIEW_DESC GetRtvDesc() { return rtvDesc; }
	// srvDescriptorHeapを取得
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> GetSrvDescriptorHeap() { return srvDescriptorHeap; }

private:
	// FPS固定初期化
	void InitializeFixFPS();
	// FPS固定更新
	void UpdateFixFPS();

	// 記録時間
	std::chrono::steady_clock::time_point reference_;

private:
	// DirectX12デバイス
	Microsoft::WRL::ComPtr<ID3D12Device> device_;
	
	// WindowsAPI
	WinApp* winApp_ = nullptr;

	// DXGIファクトリー
	Microsoft::WRL::ComPtr<IDXGIFactory7> dxgiFactory;

	// コマンド関連の変数
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator = nullptr;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList = nullptr;
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue = nullptr;

	// スワップチェーン
	Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain;
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
	Microsoft::WRL::ComPtr<ID3D12Resource> swapChainResources[2];

	// depthStencilResource
	Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilResource;

	// DescriptorSize
	uint32_t descriptorSizeRTV;
	uint32_t descriptorSizeSRV;
	uint32_t descriptorSizeDSV;
	// DescriptorHeap
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvDescriptorHeap;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvDescriptorHeap;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvDescriptorHeap;

	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};

	// ディスクリプタ * 2
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[2];

	// Fence
	Microsoft::WRL::ComPtr<ID3D12Fence> fence;
	UINT fenceValue = 0;
	HANDLE fenceEvent;

	// ビューポート
	D3D12_VIEWPORT viewport{};

	// シザー矩形
	D3D12_RECT scissorRect{};

	// dxcCompiler
	IDxcUtils* dxcUtils;
	IDxcCompiler3* dxcCompiler;
	IDxcIncludeHandler* includeHandler;

	// リソースバリア
	D3D12_RESOURCE_BARRIER barrier{};

};


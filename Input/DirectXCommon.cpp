#include "DirectXCommon.h"
#include <cassert>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

using namespace Microsoft::WRL;

void DirectXCommon::Initialize(WinApp* winApp)
{
	//NULL検出
	assert(winApp);

	//メンバ変数に記録
	this->winApp_ = winApp;

	//初期化・生成
	DviceInitialize();
	CommandInitialize();
	SwapChainCreate();
	DepthBufferCreate();
	DescriptorHeapCreate();
	RenderTargetViewInitialize();
	DepthStencilViewInitialize();
	FenceInitialize();
	ViewportRectInitialize();
	ScissorRect();
	DXCCompilerCreate();
	ImGuiInitialize();
}

void DirectXCommon::Update()
{
}

//====================================================================

void DirectXCommon::DviceInitialize()
{
	HRESULT hr;

	// --- デバッグレイヤーをオンに ---
#ifdef _DEBUG 
	// デバッグ
	Microsoft::WRL::ComPtr<ID3D12Debug1> debugController = nullptr;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
		// デバッグレイヤーを有効化する
		debugController->EnableDebugLayer();
		// さらにGPU側でもチェックを行うようにする
		debugController->SetEnableGPUBasedValidation(TRUE);
	}
#endif // DEBUG

	// --- DXGIファクトリーの生成 ---
	Microsoft::WRL::ComPtr<IDXGIFactory7> dxgiFactory = nullptr;
	hr = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory));
	assert(SUCCEEDED(hr));

	// --- アダプターの列挙 ---
	Microsoft::WRL::ComPtr<IDXGIAdapter4> useAdapter = nullptr;
	for (UINT i = 0; dxgiFactory->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&useAdapter)) != DXGI_ERROR_NOT_FOUND; ++i) {
		// アダプタの情報を取得する
		DXGI_ADAPTER_DESC3 adapterDesc{};
		hr = useAdapter->GetDesc3(&adapterDesc);
		assert(SUCCEEDED(hr));
		// ソフトウェアアダプタでなければ採用
		if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)) {
			// 採用したアダプタの情報をログに出力。wstringの方なので注意
			Log(ConvertString(std::format(L"Use Adapter : {}\n", adapterDesc.Description)));
			break;
		}
		useAdapter = nullptr;
	}
	assert(useAdapter != nullptr);
}

void DirectXCommon::CommandInitialize()
{
}

void DirectXCommon::SwapChainCreate()
{
}

void DirectXCommon::DepthBufferCreate()
{
}

void DirectXCommon::DescriptorHeapCreate()
{
}

void DirectXCommon::RenderTargetViewInitialize()
{
}

void DirectXCommon::DepthStencilViewInitialize()
{
}

void DirectXCommon::FenceInitialize()
{
}

void DirectXCommon::ViewportRectInitialize()
{
}

void DirectXCommon::ScissorRect()
{
}

void DirectXCommon::DXCCompilerCreate()
{
}

void DirectXCommon::ImGuiInitialize()
{
}

#pragma once
#include <DirectXCommon.h>

// SRV管理
class SrvManager
{
public:
	// 初期化
	void Initialize(DirectXCommon* dxCommon);

	// 確保関数
	uint32_t Allocate();
	bool IsAllocate();

	// SRV生成関数
	// テクスチャ 用
	void CreateSRVforTexture2D(uint32_t srvIndex, ID3D12Resource* pResource, DXGI_FORMAT Format, UINT MipLevels);
	// Structured Buffer 用
	void CreateSRVforStructuredBuffer(uint32_t srvIndex, ID3D12Resource* pResource, UINT numElements, UINT structureByteStride);

	// ヒープセット
	void PreDraw();


public:
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(uint32_t index);
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(uint32_t index);

	// SRVの設定
	void SetGraphicsRootDescriptorTable(UINT RootParameterIndex, uint32_t srvIndex);

public:
	// 最大SRV数(最大テクスチャ数)
	static const uint32_t kMaxSRVCount;

private:
	DirectXCommon* dxCommon_ = nullptr;

	// --- SRVヒープ ---
	//SRV用デスクリプタサイズ
	uint32_t descriptorSize;
	// SRV用デスクリプタヒープ
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap;

	// --- 確保関数 ---
	// 次に使用するSRVインデックス
	uint32_t useIndex = 0;

};


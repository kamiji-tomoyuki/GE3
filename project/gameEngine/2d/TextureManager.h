#pragma once
#include <d3d12.h>
#include <string>
#include <unordered_map>
#include <wrl.h>

#include "DirectXCommon.h"
#include "SrvManager.h"

#include "../../externals/DirectXTex/DirectXTex.h"

using namespace Microsoft::WRL;

class TextureManager
{
#pragma region シングルトンインスタンス
private:
	static TextureManager* instance;

	TextureManager() = default;
	~TextureManager() = default;
	TextureManager(TextureManager&) = delete;
	TextureManager& operator=(TextureManager&) = delete;

public:
	// シングルトンインスタンスの取得
	static TextureManager* GetInstance();
	// 終了
	void Finalize();
#pragma endregion シングルトンインスタンス

public:
	// 初期化
	void Initialize(DirectXCommon* dxCommon, SrvManager* srvManager);

	// テクスチャファイルの読み込み
	void LoadTexture(const std::string& filePath);

	std::wstring ConvertString(const std::string& str);
	std::string ConvertString(const std::wstring& str);

public:
	// メタデータの取得
	const DirectX::TexMetadata& GetMetaData(const std::string& filePath);

	// SRVインデックスの開始番号取得
	uint32_t GetTextureIndexByFilePath(const std::string& filePath);

	// テクスチャ番号からGPUハンドルを取得
	D3D12_GPU_DESCRIPTOR_HANDLE GetSrvHandleGPU(const std::string& filePath);

private:
	DirectXCommon* dxCommon;
	SrvManager* srvManager;

	// テクスチャ1枚分のデータ
	struct TextureData {
		std::string filepath;								// 画像ファイルパス
		DirectX::TexMetadata metadata;						// 画像の幅・高さ
		Microsoft::WRL::ComPtr<ID3D12Resource> resource;	// テクスチャリソース
		Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource;
		uint32_t srvIndex;
		D3D12_CPU_DESCRIPTOR_HANDLE srvHandleCPU;
		D3D12_GPU_DESCRIPTOR_HANDLE srvHandleGPU;
	};

	// テクスチャデータ
	std::unordered_map < std::string,TextureData> textureDatas;

	// SRVインデックスの開始番号
	static uint32_t kSRVIndexTop;

};


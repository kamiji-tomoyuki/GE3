#include "TextureManager.h"

TextureManager* TextureManager::instance = nullptr;

// ImGuiで0番を使用するため1番から使用
uint32_t TextureManager::kSRVIndexTop = 1;

// シングルトンインスタンスの取得・終了
TextureManager* TextureManager::GetInstance()
{
	if (instance == nullptr) {
		instance = new TextureManager;
	}
	return instance;
}
void TextureManager::Finalize()
{
	delete instance;
	instance = nullptr;
}

void TextureManager::Initialize(DirectXCommon* dxCommon, SrvManager* srvManager)
{
	// メンバ変数として記録
	this->dxCommon = dxCommon;
	this->srvManager = srvManager;

	// SRVの数と同数
	textureDatas.rehash(kSRVIndexTop < SrvManager::kMaxSRVCount);
}

void TextureManager::LoadTexture(const std::string& filePath)
{
	// --- 読み込み済みテクスチャを検索 ---
	if (textureDatas.contains(filePath)) {
		// 読み込み済みなら早期return
		return;
	}

	// テクスチャ枚数上限チェック
	assert(srvManager->IsAllocate());

	// --- ファイル読み込み ---
	DirectX::ScratchImage image{};
	std::wstring filepathW = ConvertString(filePath);
	HRESULT hr = DirectX::LoadFromWICFile(filepathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
	assert(SUCCEEDED(hr));
	DirectX::ScratchImage mipImages{};
	hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 0, mipImages);
	assert(SUCCEEDED(hr));

	// --- テクスチャデータを追加 ---
	// 追加したテクスチャデータの参照を取得
	TextureData& textureData = textureDatas[filePath];

	// --- テクスチャデータ書き込み ---
	textureData.metadata = image.GetMetadata();
	textureData.resource = dxCommon->CreateTextureResources(textureData.metadata);
	// テクスチャデータをGPUにアップロード
	textureData.intermediateResource = dxCommon->UploadTextureData(textureData.resource.Get(), image);

	// --- デスクリプタハンドルの計算 ---
	textureData.srvIndex = srvManager->Allocate();
	textureData.srvHandleCPU = srvManager->GetCPUDescriptorHandle(textureData.srvIndex);
	textureData.srvHandleGPU = srvManager->GetGPUDescriptorHandle(textureData.srvIndex);

	// --- SRVの生成 ---
	srvManager->CreateSRVforTexture2D(
		textureData.srvIndex,                // SRVインデックス
		textureData.resource.Get(),          // リソース
		textureData.metadata.format,         // フォーマット
		UINT(textureData.metadata.mipLevels) // ミップレベル
	);
}

uint32_t TextureManager::GetTextureIndexByFilePath(const std::string& filePath)
{
	// unordered_mapを使って直接インデックスを取得
	auto it = textureDatas.find(filePath);
	if (it != textureDatas.end()) {
		return it->second.srvIndex;
	}
	// なかったらエラーメッセージ
	Logger::Log("Error: Texture not found for filePath: " + filePath);
	assert(0);
	return 0;
}

D3D12_GPU_DESCRIPTOR_HANDLE TextureManager::GetSrvHandleGPU(const std::string& filePath)
{
	// テクスチャが存在するか確認
	auto it = textureDatas.find(filePath);
	if (it == textureDatas.end()) {
		// なかったらエラーメッセージ
		Logger::Log("Error: Texture not found for filePath: " + filePath);
		throw std::runtime_error("Texture not found for filePath: " + filePath);
	}

	// テクスチャデータの参照を取得
	TextureData& textureData = it->second;

	// GPUハンドルを返却
	return textureData.srvHandleGPU;
}

const DirectX::TexMetadata& TextureManager::GetMetaData(const std::string& filePath)
{
	// テクスチャが存在するか確認
	auto it = textureDatas.find(filePath);
	if (it == textureDatas.end()) {
		// なかったらエラーメッセージ
		Logger::Log("Error: Texture not found for filePath: " + filePath);
		throw std::runtime_error("Texture not found for filePath: " + filePath);
	}

	// テクスチャデータの参照を取得
	TextureData& textureData = it->second;

	// メタデータを返却
	return textureData.metadata;
}

std::wstring TextureManager::ConvertString(const std::string& str) {
	if (str.empty()) {
		return std::wstring();
	}

	auto sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(&str[0]), static_cast<int>(str.size()), NULL, 0);
	if (sizeNeeded == 0) {
		return std::wstring();
	}
	std::wstring result(sizeNeeded, 0);
	MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(&str[0]), static_cast<int>(str.size()), &result[0], sizeNeeded);
	return result;
}

std::string TextureManager::ConvertString(const std::wstring& str)
{
	if (str.empty()) {
		return std::string();
	}

	auto sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), NULL, 0, NULL, NULL);
	if (sizeNeeded == 0) {
		return std::string();
	}
	std::string result(sizeNeeded, 0);
	WideCharToMultiByte(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), result.data(), sizeNeeded, NULL, NULL);
	return result;
}
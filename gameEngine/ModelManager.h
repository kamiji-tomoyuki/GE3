#pragma once
#include <string>
#include <map>
#include <memory>

#include "Model.h"

class ModelCommon;
class DirectXCommon;

// モデルマネージャー
class ModelManager
{
#pragma region シングルトンインスタンス
private:
	static ModelManager* instance;

	ModelManager() = default;
	~ModelManager() = default;
	ModelManager(ModelManager&) = delete;
	ModelManager& operator = (ModelManager&) = delete;

public:
	// シングルトンインスタンスの取得
	static ModelManager* GetInstance();
	// 終了
	void Finalize();
#pragma endregion シングルトンインスタンス

public:
	// 初期化
	void Initialize(DirectXCommon* dxCommon);

	// モデルファイルの読み込み
	void LoadModel(const std::string& filePath);

	// モデルの検索
	Model* FindModel(const std::string& filePath);

private:
	// --- モデルデータ ---
	std::map<std::string, std::unique_ptr<Model>> models;

	// --- モデル共通部 ---
	ModelCommon* modelCommon_ = nullptr;
};


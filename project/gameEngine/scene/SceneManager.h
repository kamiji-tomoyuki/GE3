#pragma once
#include <BaseScene.h>

#include <AbstractSceneFactory.h>

// シーン管理
class SceneManager
{
#pragma region シングルトンインスタンス
private:
	static SceneManager* instance;

	SceneManager() = default;
	~SceneManager() = default;
	SceneManager(SceneManager&) = delete;
	SceneManager& operator = (SceneManager&) = delete;

public:
	// シングルトンインスタンスの取得
	static SceneManager* GetInstance();
	// 終了
	void Finalize();
#pragma endregion シングルトンインスタンス

public:
	// 更新処理
	void Update();

	// 描画処理
	void Draw();

	// 次のシーンを予約	
	void ChangeScene(const std::string& sceneName);

	// シーンファクトリーを設定
	void SetSceneFactory(AbstractSceneFactory* sceneFactory) { sceneFactory_ = sceneFactory; }


private:
	// 実行中のシーン
	BaseScene* scene_ = nullptr;
	// 次のシーン
	BaseScene* nextScene_ = nullptr;

	// シーンファクトリー
	AbstractSceneFactory* sceneFactory_ = nullptr;
};


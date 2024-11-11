#pragma once

// 前方宣言
class SceneManager;

// シーン基底クラス
class BaseScene
{
public:
	// デストラクタ
	virtual ~BaseScene() = default;

	// 初期化
	virtual void Initialize();

	// 終了
	virtual void Finalize();

	// 更新処理
	virtual void Update();

	// 描画処理
	virtual void Draw();

public:
	// シーンマネージャを設定
	virtual void SetSceneManager(SceneManager* sceneManager) { sceneManager_ = sceneManager; }

private:
	// シーンマネージャ
	SceneManager* sceneManager_ = nullptr;

};


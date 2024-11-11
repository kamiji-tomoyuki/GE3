#pragma once

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


};


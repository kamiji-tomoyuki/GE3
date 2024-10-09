#pragma once
#include "Camera.h"

// カメラマネージャ
class CameraManager
{
#pragma region シングルトンインスタンス
private:
	static CameraManager* instance;

	CameraManager() = default;
	~CameraManager() = default;
	CameraManager(CameraManager&) = delete;
	CameraManager& operator = (CameraManager&) = delete;

public:
	// シングルトンインスタンスの取得
	static CameraManager* GetInstance();
	// 終了
	void Finalize();
#pragma endregion シングルトンインスタンス
};


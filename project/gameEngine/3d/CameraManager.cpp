#include "CameraManager.h"
#include "DirectXCommon.h"

CameraManager* CameraManager::instance = nullptr;

CameraManager* CameraManager::GetInstance()
{
	if (instance == nullptr) {
		instance = new CameraManager;
	}
	return instance;
}

void CameraManager::Finalize()
{
	delete instance;
	instance = nullptr;
}

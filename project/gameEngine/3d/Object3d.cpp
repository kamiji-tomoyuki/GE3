#include "Object3d.h"
#include "Object3dCommon.h"
#include "TextureManager.h"
#include "WinApp.h"

#include <fstream>
#include <sstream>

#include "Model.h"
#include "../math/CalculateMath.h"
#include "ModelManager.h"

void Object3d::Initialize(Object3dCommon* object3dCommon)
{
	// --- 引数で受け取りメンバ変数に記録 ---
	this->object3dCommon = object3dCommon;

	// 座標変換
	TransformationMatrixResource();
	// 平行光源
	DirectionalLightResource();

	// --- Transform変数の作成 ---
	transform = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };
	
	// --- cameraの設置 ---
	this->camera = object3dCommon->GetDefaultCamera();

}

void Object3d::Update()
{
	// --- world座標変換 ---
	Matrix4x4 worldMatrix = MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);
	Matrix4x4 worldViewProjectionMatrix;
	if (camera) {
		const Matrix4x4& viewProjectionMatrix = camera->GetViewProjectionMatrix();
		worldViewProjectionMatrix = worldMatrix * viewProjectionMatrix;
	}
	else {
		worldViewProjectionMatrix = worldMatrix;
	}

	// --- transformationMatrixDataの更新 ---
	transformationMatrixData->WVP = worldViewProjectionMatrix;
	transformationMatrixData->World = worldMatrix;
}

void Object3d::Draw()
{
	// --- 座標変換行列CBufferの場所を設定 ---
	object3dCommon->GetDxCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(1, transformationMatrixResource->GetGPUVirtualAddress());

	// --- 平行光源CBufferの場所を設定 ---
	object3dCommon->GetDxCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(3, directionalLightResource->GetGPUVirtualAddress());

	// --- 描画 ---
	if (model) {
		// 3Dモデルが割り当てられていれば描画
		model->Draw();
	}
}

void Object3d::SetModel(const std::string& filePath)
{
	// モデルを検索してセット
	model = ModelManager::GetInstance()->FindModel(filePath);
}

void Object3d::TransformationMatrixResource()
{
	// --- transformationMatrixResourceの作成 ---
	transformationMatrixResource = object3dCommon->GetDxCommon()->CreateBufferResource(sizeof(TransformationMatrix));

	// --- transformationMatrixDataに割り当てる ---
	transformationMatrixResource->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixData));

	transformationMatrixData->WVP = MakeIdentity4x4();
	transformationMatrixData->World = MakeIdentity4x4();
}
void Object3d::DirectionalLightResource()
{
	// --- directionalLightResourceの作成 ---
	directionalLightResource = object3dCommon->GetDxCommon()->CreateBufferResource(sizeof(DirectionalLight));

	// --- directionalLightDataに割り当てる ---
	directionalLightResource->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightData));

	directionalLightData->color = { 1.0f, 1.0f, 1.0f, 1.0f };
	directionalLightData->direction = { 0.0f, -1.0f, 0.0f };
	directionalLightData->intensity = 1.0f;
}

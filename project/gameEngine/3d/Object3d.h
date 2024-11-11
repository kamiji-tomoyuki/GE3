#pragma once
#include <d3d12.h>
#include <numbers>
#include <string>
#include <vector>
#include <wrl.h>

#include "Camera.h"

#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4x4.h"

class Object3dCommon;
class Model;

// 3Dオブジェクト
class Object3d
{
public:
	// 初期化
	void Initialize(Object3dCommon* object3dCommon);

	// 更新処理
	void Update();

	// 描画処理
	void Draw();

public:
	// position
	const Vector3& GetPosition() const { return transform.translate; }
	void SetPosition(const Vector3& translate) { this->transform.translate = translate; }

	// rotate
	const Vector3& GetRotate() const { return transform.rotate; }
	void SetRotate(Vector3 rotate) { this->transform.rotate = rotate; }

	// scale
	const Vector3& GetSize() const { return transform.scale; }
	void SetSize(const Vector3& scale) { this->transform.scale = scale; }
	
	// model
	void SetModel(const std::string& filePath);

	// camera
	void SetCamera(Camera* camera) { this->camera = camera; }

private:
	//Data書き込み
	void TransformationMatrixResource();
	void DirectionalLightResource();

private:
	Object3dCommon* object3dCommon = nullptr;
	Model* model = nullptr;

	// --- 座標変換 ---
	struct TransformationMatrix {
		Matrix4x4 WVP;
		Matrix4x4 World;
	};
	// バッファリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> transformationMatrixResource;
	// バッファリソース内のデータを指すポインタ
	TransformationMatrix* transformationMatrixData = nullptr;

	// --- 平行光源 ---
	struct DirectionalLight {
		Vector4 color;     // ライトの色
		Vector3 direction; // ライトの向き
		float intensity;   // 輝度
	};
	// バッファリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> directionalLightResource;
	// バッファリソース内のデータを指すポインタ
	DirectionalLight* directionalLightData = nullptr;

	// --- Transform ---
	struct Transform {
		Vector3 scale;
		Vector3 rotate;
		Vector3 translate;
	};
	Transform transform;
	Camera* camera = nullptr;

};


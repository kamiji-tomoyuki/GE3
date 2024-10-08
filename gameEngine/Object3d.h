#pragma once
#include <d3d12.h>
#include <numbers>
#include <string>
#include <vector>
#include <wrl.h>

#include "../math/Vector2.h"
#include "../math/Vector3.h"
#include "../math/Vector4.h"
#include "../math/Matrix4x4.h"

class Object3dCommon;

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


private:
	//Data書き込み
	void VertexResource();
	void MaterialResource();
	void TransformationMatrixResource();
	void DirectionalLightResource();

private:
	Object3dCommon* object3dCommon = nullptr;

	// --- 頂点データ ---
	struct VertexData {
		Vector4 position;
		Vector2 texcoord;
		Vector3 normal;
	};
	const int vertexCount = 6;//頂点数
	// バッファリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource;
	// バッファリソース内のデータを指すポインタ
	VertexData* vertexData = nullptr;
	// バッファリソースの使い道を補足するバッファビュー
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};

	// --- マテリアル ---
	struct Material {
		Vector4 color;
		int32_t enableLighting;
		float padding[3];
		Matrix4x4 uvTransform;
	};
	// バッファリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource;
	// バッファリソース内のデータを指すポインタ
	Material* materialData = nullptr;

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
	Transform cameraTransform;

private:
	// --- 色データ ---
	struct Color {
		float r, g, b;
	};

	// --- マテリアルデータ ---
	struct MaterialData {
		std::string name;
		float Ns;
		Color Ka; // 環境光色
		Color Kd; // 拡散反射色
		Color Ks; // 鏡面反射光
		float Ni;
		float d;
		uint32_t illum;
		std::string textureFilePath;
		uint32_t textureIndex = 0;
	};

	// --- モデルデータ ---
	struct ModelData {
		std::vector<VertexData> vertices;
		MaterialData material;
	};
	// Objファイルのデータ
	ModelData modelData_;

	//.mtlファイル読み取り
	static MaterialData LoadMaterialTemplaterfile(const std::string& directoryPath, const std::string& filename);
	//.objファイル読み取り
	static ModelData LoadObjFile(const std::string& directoryPath, const std::string& filename);

};


#pragma once
#include <d3d12.h>
#include <string>
#include <vector>
#include <wrl.h>

#include "../math/Vector2.h"
#include "../math/Vector3.h"
#include "../math/Vector4.h"
#include "../math/Matrix4x4.h"

class ModelCommon;

// 3Dモデル
class Model
{
public:
	// 初期化
	void Initialize(ModelCommon* modelCommon, const std::string& directorypath, const std::string& filename);

	// 描画処理
	void Draw();

private:
	// ===== 構造体 =====
	// --- 頂点データ ---
	struct VertexData {
		Vector4 position;
		Vector2 texcoord;
		Vector3 normal;
	};
	// --- 色データ ---
	struct Color {
		float r, g, b;
	};
	// --- マテリアル ---
	struct Material {
		Vector4 color;
		int32_t enableLighting;
		float padding[3];
		Matrix4x4 uvTransform;
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
	// --- 座標変換 ---
	struct TransformationMatrix {
		Matrix4x4 WVP;
		Matrix4x4 World;
	};
	// --- 平行光源 ---
	struct DirectionalLight {
		Vector4 color;     // ライトの色
		Vector3 direction; // ライトの向き
		float intensity;   // 輝度
	};
	// --- Transform ---
	struct Transform {
		Vector3 scale;
		Vector3 rotate;
		Vector3 translate;
	};
	// --- モデルデータ ---
	struct ModelData {
		std::vector<VertexData> vertices;
		MaterialData material;
	};

private:
	//Data書き込み
	void VertexResource();
	void MaterialResource();

	// .mtlファイルの読み取り
	static MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename);
	//.objファイルの読み取り
	static ModelData LoadObjFile(const std::string& directoryPath, const std::string& filename);

private:
	// --- ModelCommon ---
	ModelCommon* modelCommon_;

	// --- Objファイル ---
	ModelData modelData_;

	// --- バッファリソース ---
	// VertexResource
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource;
	// VertexBufferView
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	// VertexResourceにデータを書き込むためのポインタ
	VertexData* vertexData = nullptr;

	// --- マテリアル ---
	// マテリアルリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource;
	// マテリアルリソースにデータを書き込むためのポインタ
	Material* materialData = nullptr;
	
	// --- Transform ---
	Transform transform;
};


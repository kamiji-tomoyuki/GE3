#pragma once
#pragma once
#include <wrl.h>
#include <numbers>

#include "SpriteCommon.h"

#include "../Math/Vector2.h"
#include "../Math/Vector3.h"
#include "../Math/Vector4.h"
#include "../Math/Matrix4x4.h"
#include "../Math/CalculateMath.h"

class SpriteCommon;

//スプライト
class Sprite
{
public://メンバ関数
	//初期化
	void Initialize(SpriteCommon* spriteCommon);

	//更新処理
	void Update();

	//描画処理
	void Draw();

private:
	//Data書き込み
	void VertexDataWriting();
	void IndexDataWriting();
	void MaterialDataWriting();
	void TransformationMatrixDataWriting();

	//worldMatrixの作成
	void MakeWorldMatrix();

private:
	SpriteCommon* spriteCommon = nullptr;

	// --- 頂点データ ---
	struct VertexData {
		Vector4 position;
		Vector2 texcoord;
		Vector3 normal;
	};
	const int vertexCount = 6;//頂点数
	//バッファリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource;
	Microsoft::WRL::ComPtr<ID3D12Resource> indexResource;
	//バッファリソース内のデータを指すポインタ
	VertexData* vertexData = nullptr;
	uint32_t* indexData = nullptr;
	//バッファリソースの使い道を補足するバッファビュー
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	D3D12_INDEX_BUFFER_VIEW indexBufferView{};

	// --- マテリアルデータ ---
	struct Material {
		Vector4 color;
		int32_t enableLighting;
		float padding[3];
		Matrix4x4 uvTransform;
	};
	//バッファリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource;
	//バッファリソース内のデータを指すポインタ
	Material* materialData = nullptr;

	// --- 座標変換 ---
	struct TransformationMatrix {
		Matrix4x4 WVP;
		Matrix4x4 World;
	};
	//バッファリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> transformationMatrixResource;
	//バッファリソース内のデータを指すポインタ
	TransformationMatrix* transformationMatrixData = nullptr;

	// --- world座標変換 ---
	struct Transform {
		Vector3 scale;
		Vector3 rotate;
		Vector3 translate;
	};
	Transform cameraTransform{
		{1.0f, 1.0f, 1.0f  },
		{0.0f, 0.0f, 0.0f  },
		{0.0f, 23.0f, 10.0f},
	};
	Matrix4x4 backToFrontMatrix = MakeRotateYMatrix(std::numbers::pi_v<float>);
	Matrix4x4 viewMatrix;
	Matrix4x4 projectionMatrix;
	Matrix4x4 worldMatrix;

	// --- texture ---
	Microsoft::WRL::ComPtr<ID3D12Resource> textureResource;
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU;
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU;
};


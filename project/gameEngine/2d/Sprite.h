#pragma once
#pragma once
#include <wrl.h>
#include <numbers>

#include "SpriteCommon.h"

#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4x4.h"
#include "CalculateMath.h"

class SpriteCommon;

//スプライト
class Sprite
{
public://メンバ関数
	//初期化
	void Initialize(SpriteCommon* spriteCommon, std::string textureFilePath);

	//更新処理
	void Update();

	//描画処理
	void Draw();

public:
	// position
	const Vector2& GetPosition() const { return position; }
	void SetPosition(const Vector2& position) { this->position = position; }

	// rotate
	float GetRotate() const { return rotation; }
	void SetRotate(float rotation) { this->rotation = rotation; }

	// scale
	const Vector2& GetSize() const { return size; }
	void SetSize(const Vector2& size) { this->size = size; }

	// color
	const Vector4& GetColor() const { return materialData->color; }
	void SetColor(const Vector4& color) { materialData->color = color; }

	// anchorPoint
	const Vector2& GetAnchorPoint() const { return anchorPoint; }
	void SetAnchorPoint(const Vector2& anchorPoint) { this->anchorPoint = anchorPoint; }

	// flip
	bool GetIsFlipX() const { return isFlipX_; }
	bool GetIsFlipY() const { return isFlipY_; }
	void SetFlipX(bool isFlipX) { this->isFlipX_ = isFlipX; }
	void SetFlipY(bool isFlipY) { this->isFlipY_ = isFlipY; }

	// テクスチャ範囲指定
	const Vector2& GetTextureLeftTop() const { return textureLeftTop; }
	void SetTextureLeftTop(const Vector2& textureLeftTop) { this->textureLeftTop = textureLeftTop; }
	const Vector2& GetTextureSize() const { return textureSize; }
	void SetTextureSize(const Vector2& textureSize) { this->textureSize = textureSize; }

private:
	//Data書き込み
	void VertexDataWriting();
	void IndexDataWriting();
	void MaterialDataWriting();
	void TransformationMatrixDataWriting();

	// テクスチャサイズをイメージに合わせる
	void AdjustTextureSize();

private:
	SpriteCommon* spriteCommon = nullptr;
	std::string textureFilePath_;

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

	Transform transform{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };
	Vector2 position = { 0.0f,0.0f };
	float rotation = 0.0f;
	Vector2 size = { 640.0f,360.0f };
	
	// --- texture ---
	Microsoft::WRL::ComPtr<ID3D12Resource> textureResource;
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU;
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU;
	// テクスチャ番号
	uint32_t textureIndex = 0;

	// --- アンカーポイント ---
	Vector2 anchorPoint = { 0.0f,0.0f };
	float left = 0.0f - anchorPoint.x;
	float right = 1.0f - anchorPoint.x;
	float top = 0.0f - anchorPoint.y;
	float bottom = 1.0f - anchorPoint.y;

	// --- フリップ ---
	// 左右
	bool isFlipX_ = false;
	// 上下
	bool isFlipY_ = false;

	// --- テクスチャ範囲指定 ---
	// 左上座標
	Vector2 textureLeftTop = { 0.0f,0.0f };
	// 切り出しサイズ
	Vector2 textureSize = { 400.0f,400.0f };
	float tex_left;
	float tex_right;
	float tex_top;
	float tex_bottom;

};


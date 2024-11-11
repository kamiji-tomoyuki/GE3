#include "Sprite.h"
#include "SpriteCommon.h"
#include "TextureManager.h"
#include "WinApp.h"

void Sprite::Initialize(SpriteCommon* spriteCommon, std::string textureFilePath)
{
	// --- 引数で受け取りメンバ変数に記録 ---
	this->spriteCommon = spriteCommon;
	this->textureFilePath_ = textureFilePath;

#pragma region 頂点データ
	// --- vertexResourceの作成 ---
	vertexResource = spriteCommon->GetDxCommon()->CreateBufferResource(sizeof(VertexData) * vertexCount);
	// --- indexResourceの作成 ---
	indexResource = spriteCommon->GetDxCommon()->CreateBufferResource(sizeof(uint32_t) * vertexCount);

	// --- vertexBufferViewの作成 ---
	vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
	vertexBufferView.SizeInBytes = sizeof(VertexData) * vertexCount;
	vertexBufferView.StrideInBytes = sizeof(VertexData);
	// --- indexBufferViewの作成 ---
	indexBufferView.BufferLocation = indexResource->GetGPUVirtualAddress();
	indexBufferView.SizeInBytes = sizeof(uint32_t) * vertexCount;
	indexBufferView.Format = DXGI_FORMAT_R32_UINT;

	// --- vertexDataに割り当てる ---
	vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
	VertexDataWriting();
	// --- indexDataに割り当てる ---
	indexResource->Map(0, nullptr, reinterpret_cast<void**>(&indexData));
	IndexDataWriting();
#pragma endregion 頂点データ

#pragma region マテリアルデータ
	// --- materialResourceの作成 ---
	materialResource = spriteCommon->GetDxCommon()->CreateBufferResource(sizeof(Material));

	// --- materialDataに割り当てる ---
	materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
	MaterialDataWriting();
#pragma endregion マテリアルデータ

#pragma region 座標変換
	// --- transformationMatrixResourceの作成 ---
	transformationMatrixResource = spriteCommon->GetDxCommon()->CreateBufferResource(sizeof(TransformationMatrix));

	// --- transformationMatrixDataに割り当てる ---
	transformationMatrixResource->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixData));
	TransformationMatrixDataWriting();
#pragma endregion 座標変換

	// --- テクスチャ読み込み ---
	TextureManager::GetInstance()->LoadTexture(textureFilePath_);

	// --- 単位行列 ---
	textureIndex = TextureManager::GetInstance()->GetTextureIndexByFilePath(textureFilePath_);

	// --- 切り取り ---
	AdjustTextureSize();

}

void Sprite::Update()
{
	// --- world座標変換 ---
	transform.translate = { position.x, position.y, 0.0f };
	transform.rotate = { 0.0f, 0.0f, rotation };
	transform.scale = { size.x, size.y, 1.0f };
	worldMatrix = MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);
	viewMatrix = MakeIdentity4x4();
	projectionMatrix = MakeOrthographicMatrix(0.0f, 0.0f, float(WinApp::kClientWidth), float(WinApp::kClientHeight), 0.0f, 100.0f);

	// --- transformationMatrixDataの更新 ---
	transformationMatrixData->WVP = worldMatrix * viewMatrix * projectionMatrix;
	transformationMatrixData->World = worldMatrix;

	// --- フリップの更新処理 ---
	if (isFlipX_) {
		left = -left;
		right = -right;
	}
	if (isFlipY_) {
		top = -top;
		bottom = -bottom;
	}

	// --- テクスチャ範囲指定の更新処理 ---
	const DirectX::TexMetadata& metadata =
		TextureManager::GetInstance()->GetMetaData(textureFilePath_);
	tex_left = textureLeftTop.x / metadata.width;
	tex_right = (textureLeftTop.x + textureSize.x) / metadata.width;
	tex_top = textureLeftTop.y / metadata.height;
	tex_bottom = (textureLeftTop.y + textureSize.y) / metadata.height;
	// 適用
	VertexDataWriting();

}

void Sprite::Draw()
{
	// --- vertexBufferViewの生成 ---
	spriteCommon->GetDxCommon()->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView);
	// --- indexBufferViewの生成 ---
	spriteCommon->GetDxCommon()->GetCommandList()->IASetIndexBuffer(&indexBufferView);

	// --- マテリアルCBufferの場所を設定 --- 
	spriteCommon->GetDxCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource->GetGPUVirtualAddress());

	// --- 座標変換行列CBufferの場所を設定 ---
	spriteCommon->GetDxCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(1, transformationMatrixResource->GetGPUVirtualAddress());

	// --- SRVのDescriptorTableを設定 ---
	spriteCommon->GetDxCommon()->GetCommandList()->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->GetSrvHandleGPU(textureFilePath_));

	// --- 描画(DrawCall/ドローコール) ---
	spriteCommon->GetDxCommon()->GetCommandList()->DrawIndexedInstanced(vertexCount, 1, 0, 0, 0);

}

void Sprite::VertexDataWriting()
{
	vertexData[0].position = { left, bottom, 0.0f, 1.0f }; // 左下
	vertexData[0].texcoord = { tex_left, tex_bottom };

	vertexData[1].position = { left, top, 0.0f, 1.0f }; // 左上
	vertexData[1].texcoord = { tex_left, tex_top };

	vertexData[2].position = { right, bottom, 0.0f, 1.0f }; // 右下
	vertexData[2].texcoord = { tex_right, tex_bottom };

	vertexData[3].position = { right, top, 0.0f, 1.0f }; // 右上
	vertexData[3].texcoord = { tex_right, tex_top };
}
void Sprite::IndexDataWriting()
{
	indexData[0] = 0;
	indexData[1] = 1;
	indexData[2] = 2;

	indexData[3] = 1;
	indexData[4] = 3;
	indexData[5] = 2;
}

void Sprite::MaterialDataWriting()
{
	materialData->color = { 1.0f, 1.0f, 1.0f, 1.0f };
	materialData->enableLighting = false;
	materialData->uvTransform = MakeIdentity4x4();
}

void Sprite::TransformationMatrixDataWriting()
{
	transformationMatrixData->WVP = MakeIdentity4x4();
	transformationMatrixData->World = MakeIdentity4x4();
}

void Sprite::AdjustTextureSize()
{
	// テクスチャメタデータを取得
	const DirectX::TexMetadata& metadata = TextureManager::GetInstance()->GetMetaData(textureFilePath_);

	textureSize.x = static_cast<float>(metadata.width);
	textureSize.y = static_cast<float>(metadata.height);
	// 画像サイズをテクスチャサイズに合わせる
	size = textureSize;
}

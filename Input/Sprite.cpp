#include "Sprite.h"
#include "SpriteCommon.h"
#include "WinApp.h"

void Sprite::Initialize(SpriteCommon* spriteCommon)
{
	// --- 引数で受け取りメンバ変数に記録 ---
	this->spriteCommon = spriteCommon;

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

}

void Sprite::Update()
{
	// --- 頂点データ書き込み ---
	//spriteCommon->GetDxCommon()->GetCommandList()->DrawInstanced(4, 1, 0, 0);

	// --- インデックスリソース書き込み ---
	//spriteCommon->GetDxCommon()->GetCommandList()->DrawIndexedInstanced(vertexCount, 1, 0, 0, 0);

	// --- world座標変換 ---
	MakeWorldMatrix();

	// --- transformationMatrixDataの更新 ---
	transformationMatrixData->WVP = Multiply(worldMatrix, Multiply(viewMatrix, projectionMatrix));
	transformationMatrixData->World = worldMatrix;

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
	spriteCommon->GetDxCommon()->GetCommandList()->SetGraphicsRootDescriptorTable(2, spriteCommon->GetDxCommon()->GetSRVGPUDescriptorHandle(2));

	// --- 描画(DrawCall/ドローコール) ---
	spriteCommon->GetDxCommon()->GetCommandList()->DrawIndexedInstanced(vertexCount, 1, 0, 0, 0);

}

void Sprite::VertexDataWriting()
{
	vertexData[0].position = { 0.0f, 360.0f, 0.0f, 1.0f }; // 左下
	vertexData[0].texcoord = { 0.0f, 1.0f };

	vertexData[1].position = { 0.0f, 0.0f, 0.0f, 1.0f }; // 左上
	vertexData[1].texcoord = { 0.0f, 0.0f };

	vertexData[2].position = { 640.0f, 360.0f, 0.0f, 1.0f }; // 右下
	vertexData[2].texcoord = { 1.0f, 1.0f };

	vertexData[3].position = { 640.0f, 0.0f, 0.0f, 1.0f }; // 右上
	vertexData[3].texcoord = { 1.0f, 0.0f };
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

void Sprite::MakeWorldMatrix()
{
	Transform transform{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };

	worldMatrix = MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);
	viewMatrix = MakeIdentity4x4();
	projectionMatrix = MakeOrthographicMatrix(0.0f, 0.0f, float(WinApp::kClientWidth), float(WinApp::kClientHeight), 0.0f, 100.0f);

}
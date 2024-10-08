#include "Object3d.h"
#include "Object3dCommon.h"
#include "TextureManager.h"
#include "WinApp.h"

#include <fstream>
#include <sstream>

#include "../math/CalculateMath.h"

void Object3d::Initialize(Object3dCommon* object3dCommon)
{
	// --- 引数で受け取りメンバ変数に記録 ---
	this->object3dCommon = object3dCommon;

	// --- オブジェクト読み込み ---
	modelData_ = LoadObjFile("resources/models", "axis.obj");

	// 頂点データ
	VertexResource();
	// マテリアルデータ
	MaterialResource();
	// 座標変換
	TransformationMatrixResource();
	// 平行光源
	DirectionalLightResource();

	// --- .objの参照しているテクスチャファイル読み込み ---
	TextureManager::GetInstance()->LoadTexture(modelData_.material.textureFilePath);
	// 読み込んだテクスチャファイルの番号を取得
	modelData_.material.textureIndex =
		TextureManager::GetInstance()->GetTextureIndexByFilePath(modelData_.material.textureFilePath);

	// --- Transform変数の作成 ---
	transform = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };
	cameraTransform = { {1.0f,1.0f,1.0f},{0.3f,0.0f,0.0f},{0.0f,4.0f,-10.0f} };

}

void Object3d::Update()
{
	// --- world座標変換 ---
	Matrix4x4 worldMatrix = MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);
	Matrix4x4 cameraMatrix = MakeAffineMatrix(cameraTransform.scale, cameraTransform.rotate, cameraTransform.translate);
	Matrix4x4 viewMatrix = Inverse(cameraMatrix);
	Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(WinApp::kClientWidth) / float(WinApp::kClientHeight), 0.1f, 100.0f);

	// --- transformationMatrixDataの更新 ---
	transformationMatrixData->WVP = worldMatrix * viewMatrix * projectionMatrix;
	transformationMatrixData->World = worldMatrix;
}

void Object3d::Draw()
{
	// --- vertexBufferViewの生成 ---
	object3dCommon->GetDxCommon()->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView);

	// --- マテリアルCBufferの場所を設定 --- 
	object3dCommon->GetDxCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource->GetGPUVirtualAddress());

	// --- 座標変換行列CBufferの場所を設定 ---
	object3dCommon->GetDxCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(1, transformationMatrixResource->GetGPUVirtualAddress());

	// --- SRVのDescriptorTableを設定 ---
	object3dCommon->GetDxCommon()->GetCommandList()->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->GetSrvHandleGPU(modelData_.material.textureIndex));

	// --- 平行光源CBufferの場所を設定 ---
	object3dCommon->GetDxCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(3, directionalLightResource->GetGPUVirtualAddress());

	// --- 描画(DrawCall/ドローコール) ---
	object3dCommon->GetDxCommon()->GetCommandList()->DrawInstanced(UINT(modelData_.vertices.size()), 1, 0, 0);

}

void Object3d::VertexResource()
{
	// --- vertexResourceの作成 ---
	vertexResource = object3dCommon->GetDxCommon()->CreateBufferResource(sizeof(VertexData) * modelData_.vertices.size());

	// --- vertexBufferViewの作成 ---
	vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
	vertexBufferView.SizeInBytes = UINT(sizeof(VertexData) * modelData_.vertices.size());
	vertexBufferView.StrideInBytes = sizeof(VertexData);

	// --- vertexDataに割り当てる ---
	vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
	memcpy(vertexData, modelData_.vertices.data(), sizeof(VertexData) * modelData_.vertices.size());
	
}
void Object3d::MaterialResource()
{
	// --- materialResourceの作成 ---
	materialResource = object3dCommon->GetDxCommon()->CreateBufferResource(sizeof(Material));

	// --- materialDataに割り当てる ---
	materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialData));

	materialData->color = { 1.0f, 1.0f, 1.0f, 1.0f };
	materialData->enableLighting = false;
	materialData->uvTransform = MakeIdentity4x4();
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

Object3d::MaterialData Object3d::LoadMaterialTemplaterfile(const std::string& directoryPath, const std::string& filename)
{
	MaterialData materialData;
	std::string line;
	std::ifstream file(directoryPath + "/" + filename);
	assert(file.is_open());

	while (std::getline(file, line)) {
		std::string identifier;
		std::istringstream s(line);
		s >> identifier;

		if (identifier == "newmtl") {
			s >> materialData.name;
		}
		else if (identifier == "Ns") {
			s >> materialData.Ns;
		}
		else if (identifier == "Ka") {
			s >> materialData.Ka.r >> materialData.Ka.g >> materialData.Ka.b;
		}
		else if (identifier == "Kd") {
			s >> materialData.Kd.r >> materialData.Kd.g >> materialData.Kd.b;
		}
		else if (identifier == "Ks") {
			s >> materialData.Ks.r >> materialData.Ks.g >> materialData.Ks.b;
		}
		else if (identifier == "Ni") {
			s >> materialData.Ni;
		}
		else if (identifier == "d") {
			s >> materialData.d;
		}
		else if (identifier == "illum") {
			s >> materialData.illum;
		}
		else if (identifier == "map_Kd") {
			std::string textureFilename;
			s >> textureFilename;
			materialData.textureFilePath = directoryPath + "/" + textureFilename;
		}
	}
	return materialData;
}
Object3d::ModelData Object3d::LoadObjFile(const std::string& directoryPath, const std::string& filename)
{
	ModelData modelData;
	std::vector<Vector4> positions;
	std::vector<Vector3> normals;
	std::vector<Vector2> texcoords;
	std::string line;

	std::ifstream file(directoryPath + "/" + filename);
	assert(file.is_open());

	while (std::getline(file, line)) {
		std::string identifier;
		std::istringstream s(line);
		s >> identifier;

		if (identifier == "v") {
			Vector4 position;
			s >> position.x >> position.y >> position.z;
			position.x *= -1.0f;
			position.w = 1.0f;
			positions.push_back(position);
		}
		else if (identifier == "vt") {
			Vector2 texcoord;
			s >> texcoord.x >> texcoord.y;
			texcoord.y = 1.0f - texcoord.y;
			texcoords.push_back(texcoord);
		}
		else if (identifier == "vn") {
			Vector3 normal;
			s >> normal.x >> normal.y >> normal.z;
			normal.x *= -1.0f;
			normals.push_back(normal);
		}
		else if (identifier == "f") {
			VertexData triangle[3];
			for (int32_t faceVertex = 0; faceVertex < 3; ++faceVertex) {
				std::string vertexDefinition;
				s >> vertexDefinition;
				std::istringstream v(vertexDefinition);
				uint32_t elementIndices[3];
				for (int32_t element = 0; element < 3; ++element) {
					std::string index;
					std::getline(v, index, '/');
					elementIndices[element] = std::stoi(index);
				}
				Vector4 position = positions[elementIndices[0] - 1];
				Vector2 texcoord = texcoords[elementIndices[1] - 1];
				Vector3 normal = normals[elementIndices[2] - 1];
				VertexData vertex = { position, texcoord, normal };
				triangle[faceVertex] = vertex;
			}
			modelData.vertices.push_back(triangle[2]);
			modelData.vertices.push_back(triangle[1]);
			modelData.vertices.push_back(triangle[0]);
		}
		else if (identifier == "mtllib") {
			std::string materialFilename;
			s >> materialFilename;
			modelData.material = LoadMaterialTemplaterfile(directoryPath, materialFilename);
		}
	}

	return modelData;
}





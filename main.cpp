#include <Windows.h>

#include <corecrt_math_defines.h>
#include <cstdint>

#include <dxcapi.h>

#include <dxgidebug.h>
#include <format>
#include <fstream>
#include <sstream>
#include <string>

#include "externals/imgui/imgui_impl_dx12.h"
#include "externals/imgui/imgui_impl_win32.h"

#include "externals/DirectXTex/DirectXTex.h"

#include "Input/Input.h"
#include "Input/WinApp.h"
#include "Input/DirectXCommon.h"

#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dxcompiler.lib")


struct Vector2 {
	float x;
	float y;
};
struct Vector3 {
	float x;
	float y;
	float z;
};
struct Vector4 {
	float x;
	float y;
	float z;
	float w;
};

struct Matrix3x3 {
	float m[3][3];
};
struct Matrix4x4 {
	float m[4][4];
};

struct Transform {
	Vector3 scale;
	Vector3 rotate;
	Vector3 translate;
};

struct VertexData {
	Vector4 position;
	Vector2 texcoord;
	Vector3 normal;
};

struct Material {
	Vector4 color;
	bool enableLighting;
	float padding[3];
	Matrix4x4 uvTransform;
};
Transform uvTransformSprite{
	{1.0f, 1.0f, 1.0f},
	{0.0f, 0.0f, 0.0f},
	{0.0f, 0.0f, 0.0f},
};

struct TransformationMatrix {
	Matrix4x4 WVP;
	Matrix4x4 world;
};

struct DirectionalLight {
	Vector4 color;     // ライトの色
	Vector3 direction; // ライトの向き
	float intensity;   // 輝度
};

struct Color {
	float r, g, b;
};

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
};

struct ModelData {
	std::vector<VertexData> vertices;
	MaterialData material;
};


struct D3DResourceLeakChecker {
	~D3DResourceLeakChecker()
	{
		// リリースリークチェック
		Microsoft::WRL::ComPtr < IDXGIDebug1> debug;
		if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug)))) {
			debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
			debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL); // リリース不足
			debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
		}
	}
};

/////////////////////////////////////////////////////////////////////////////

//void Log(const std::string& message);
//// string->wstring
//std::wstring ConvertString(const std::string& str);
//// wstring->string
//std::string ConvertString(const std::wstring& str);

IDxcBlob* CompileShader(
	// CompilerするShaderファイルへのパス
	const std::wstring& filePath,
	// Compilerに使用するProfile
	const wchar_t* profile,
	// 初期化で生成したもの3つ
	IDxcUtils* dxcUtils, IDxcCompiler3* dxcCompiler, IDxcIncludeHandler* IDxcIncludeHandler);

Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(Microsoft::WRL::ComPtr<ID3D12Device> device, size_t sizeInBytes);
//Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap(Microsoft::WRL::ComPtr<ID3D12Device> device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible);

MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename);
ModelData LoadObjFile(const std::string& directoryPath, const std::string& filename);

// 平行移動行列
Matrix4x4 MakeTranslateMatrix(const Vector3& translate) {
	Matrix4x4 result;

	result.m[0][0] = 1;
	result.m[0][1] = 0;
	result.m[0][2] = 0;
	result.m[0][3] = 0;

	result.m[1][0] = 0;
	result.m[1][1] = 1;
	result.m[1][2] = 0;
	result.m[1][3] = 0;

	result.m[2][0] = 0;
	result.m[2][1] = 0;
	result.m[2][2] = 1;
	result.m[2][3] = 0;

	result.m[3][0] = translate.x;
	result.m[3][1] = translate.y;
	result.m[3][2] = translate.z;
	result.m[3][3] = 1;

	return result;
}
// 拡大縮小行列
Matrix4x4 MakeScaleMatrix(const Vector3& scale) {
	Matrix4x4 result;

	result.m[0][0] = scale.x;
	result.m[0][1] = 0;
	result.m[0][2] = 0;
	result.m[0][3] = 0;

	result.m[1][0] = 0;
	result.m[1][1] = scale.y;
	result.m[1][2] = 0;
	result.m[1][3] = 0;

	result.m[2][0] = 0;
	result.m[2][1] = 0;
	result.m[2][2] = scale.z;
	result.m[2][3] = 0;

	result.m[3][0] = 0;
	result.m[3][1] = 0;
	result.m[3][2] = 0;
	result.m[3][3] = 1;

	return result;
}
// 回転行列
Matrix4x4 MakeRotateZMatrix(float radian) {

	Matrix4x4 result;

	result.m[0][0] = std::cos(radian);
	result.m[0][1] = std::sin(radian);
	result.m[0][2] = 0;
	result.m[0][3] = 0;

	result.m[1][0] = -std::sin(radian);
	result.m[1][1] = std::cos(radian);
	result.m[1][2] = 0;
	result.m[1][3] = 0;

	result.m[2][0] = 0;
	result.m[2][1] = 0;
	result.m[2][2] = 1;
	result.m[2][3] = 0;

	result.m[3][0] = 0;
	result.m[3][1] = 0;
	result.m[3][2] = 0;
	result.m[3][3] = 1;

	return result;
}
Matrix4x4 MakeRotateXMatrix(float radian) {

	Matrix4x4 result;

	result.m[0][0] = 1;
	result.m[0][1] = 0;
	result.m[0][2] = 0;
	result.m[0][3] = 0;

	result.m[1][0] = 0;
	result.m[1][1] = std::cos(radian);
	result.m[1][2] = std::sin(radian);
	result.m[1][3] = 0;

	result.m[2][0] = 0;
	result.m[2][1] = -std::sin(radian);
	result.m[2][2] = std::cos(radian);
	result.m[2][3] = 0;

	result.m[3][0] = 0;
	result.m[3][1] = 0;
	result.m[3][2] = 0;
	result.m[3][3] = 1;

	return result;
}
Matrix4x4 MakeRotateYMatrix(float radian) {

	Matrix4x4 result;

	result.m[0][0] = std::cos(radian);
	result.m[0][1] = 0;
	result.m[0][2] = -std::sin(radian);
	result.m[0][3] = 0;

	result.m[1][0] = 0;
	result.m[1][1] = 1;
	result.m[1][2] = 0;
	result.m[1][3] = 0;

	result.m[2][0] = std::sin(radian);
	result.m[2][1] = 0;
	result.m[2][2] = std::cos(radian);
	result.m[2][3] = 0;

	result.m[3][0] = 0;
	result.m[3][1] = 0;
	result.m[3][2] = 0;
	result.m[3][3] = 1;

	return result;
}

// 行列の積
Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4& m2) {
	Matrix4x4 result{};
	float buf;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			buf = 0;
			for (int k = 0; k < 4; k++) {
				buf = buf + m1.m[i][k] * m2.m[k][j];
				result.m[i][j] = buf;
			}
		}
	}

	return result;
}
// 単位行列
Matrix4x4 MakeIdentity() {
	Matrix4x4 result{};
	for (int i = 0; i < 4; i++) {
		result.m[i][i] = 1;
	}

	return result;
}
// 逆行列
Matrix4x4 Inverse(const Matrix4x4& m) {
	Matrix4x4 result{};

	Matrix4x4 a{};
	a.m[0][0] = m.m[1][1] * m.m[2][2] * m.m[3][3] + m.m[1][2] * m.m[2][3] * m.m[3][1] + m.m[1][3] * m.m[2][1] * m.m[3][2] - m.m[1][3] * m.m[2][2] * m.m[3][1] - m.m[1][2] * m.m[2][1] * m.m[3][3] -
		m.m[1][1] * m.m[2][3] * m.m[3][2];

	a.m[0][1] = -(m.m[0][1] * m.m[2][2] * m.m[3][3]) - (m.m[0][2] * m.m[2][3] * m.m[3][1]) - (m.m[0][3] * m.m[2][1] * m.m[3][2]) + m.m[0][3] * m.m[2][2] * m.m[3][1] +
		m.m[0][2] * m.m[2][1] * m.m[3][3] + m.m[0][1] * m.m[2][3] * m.m[3][2];

	a.m[0][2] = m.m[0][1] * m.m[1][2] * m.m[3][3] + m.m[0][2] * m.m[1][3] * m.m[3][1] + m.m[0][3] * m.m[1][1] * m.m[3][2] - m.m[0][3] * m.m[1][2] * m.m[3][1] - m.m[0][2] * m.m[1][1] * m.m[3][3] -
		m.m[0][1] * m.m[1][3] * m.m[3][2];

	a.m[0][3] = -(m.m[0][1] * m.m[1][2] * m.m[2][3]) - (m.m[0][2] * m.m[1][3] * m.m[2][1]) - (m.m[0][3] * m.m[1][1] * m.m[2][2]) + m.m[0][3] * m.m[1][2] * m.m[2][1] +
		m.m[0][2] * m.m[1][1] * m.m[2][3] + m.m[0][1] * m.m[1][3] * m.m[2][2];

	a.m[1][0] = -(m.m[1][0] * m.m[2][2] * m.m[3][3]) - (m.m[1][2] * m.m[2][3] * m.m[3][0]) - (m.m[1][3] * m.m[2][0] * m.m[3][2]) + m.m[1][3] * m.m[2][2] * m.m[3][0] +
		m.m[1][2] * m.m[2][0] * m.m[3][3] + m.m[1][0] * m.m[2][3] * m.m[3][2];

	a.m[1][1] = m.m[0][0] * m.m[2][2] * m.m[3][3] + m.m[0][2] * m.m[2][3] * m.m[3][0] + m.m[0][3] * m.m[2][0] * m.m[3][2] - m.m[0][3] * m.m[2][2] * m.m[3][0] - m.m[0][2] * m.m[2][0] * m.m[3][3] -
		m.m[0][0] * m.m[2][3] * m.m[3][2];

	a.m[1][2] = -(m.m[0][0] * m.m[1][2] * m.m[3][3]) - (m.m[0][2] * m.m[1][3] * m.m[3][0]) - (m.m[0][3] * m.m[1][0] * m.m[3][2]) + m.m[0][3] * m.m[1][2] * m.m[3][0] +
		m.m[0][2] * m.m[1][0] * m.m[3][3] + m.m[0][0] * m.m[1][3] * m.m[3][2];

	a.m[1][3] = m.m[0][0] * m.m[1][2] * m.m[2][3] + m.m[0][2] * m.m[1][3] * m.m[2][0] + m.m[0][3] * m.m[1][0] * m.m[2][2] - m.m[0][3] * m.m[1][2] * m.m[2][0] - m.m[0][2] * m.m[1][0] * m.m[2][3] -
		m.m[0][0] * m.m[1][3] * m.m[2][2];

	a.m[2][0] = m.m[1][0] * m.m[2][1] * m.m[3][3] + m.m[1][1] * m.m[2][3] * m.m[3][0] + m.m[1][3] * m.m[2][0] * m.m[3][1] - m.m[1][3] * m.m[2][1] * m.m[3][0] - m.m[1][1] * m.m[2][0] * m.m[3][3] -
		m.m[1][0] * m.m[2][3] * m.m[3][1];

	a.m[2][1] = -(m.m[0][0] * m.m[2][1] * m.m[3][3]) - (m.m[0][1] * m.m[2][3] * m.m[3][0]) - (m.m[0][3] * m.m[2][0] * m.m[3][1]) + m.m[0][3] * m.m[2][1] * m.m[3][0] +
		m.m[0][1] * m.m[2][0] * m.m[3][3] + m.m[0][0] * m.m[2][3] * m.m[3][1];

	a.m[2][2] = m.m[0][0] * m.m[1][1] * m.m[3][3] + m.m[0][1] * m.m[1][3] * m.m[3][0] + m.m[0][3] * m.m[1][0] * m.m[3][1] - m.m[0][3] * m.m[1][1] * m.m[3][0] - m.m[0][1] * m.m[1][0] * m.m[3][3] -
		m.m[0][0] * m.m[1][3] * m.m[3][1];

	a.m[2][3] = -(m.m[0][0] * m.m[1][1] * m.m[2][3]) - (m.m[0][1] * m.m[1][3] * m.m[2][0]) - (m.m[0][3] * m.m[1][0] * m.m[2][1]) + m.m[0][3] * m.m[1][1] * m.m[2][0] +
		m.m[0][1] * m.m[1][0] * m.m[2][3] + m.m[0][0] * m.m[1][3] * m.m[2][1];

	a.m[3][0] = -(m.m[1][0] * m.m[2][1] * m.m[3][2]) - (m.m[1][1] * m.m[2][2] * m.m[3][0]) - (m.m[1][2] * m.m[2][0] * m.m[3][1]) + m.m[1][2] * m.m[2][1] * m.m[3][0] +
		m.m[1][1] * m.m[2][0] * m.m[3][2] + m.m[1][0] * m.m[2][2] * m.m[3][1];

	a.m[3][1] = m.m[0][0] * m.m[2][1] * m.m[3][2] + m.m[0][1] * m.m[2][2] * m.m[3][0] + m.m[0][2] * m.m[2][0] * m.m[3][1] - m.m[0][2] * m.m[2][1] * m.m[3][0] - m.m[0][1] * m.m[2][0] * m.m[3][2] -
		m.m[0][0] * m.m[2][2] * m.m[3][1];

	a.m[3][2] = -(m.m[0][0] * m.m[1][1] * m.m[3][2]) - (m.m[0][1] * m.m[1][2] * m.m[3][0]) - (m.m[0][2] * m.m[1][0] * m.m[3][1]) + m.m[0][2] * m.m[1][1] * m.m[3][0] +
		m.m[0][1] * m.m[1][0] * m.m[3][2] + m.m[0][0] * m.m[1][2] * m.m[3][1];

	a.m[3][3] = m.m[0][0] * m.m[1][1] * m.m[2][2] + m.m[0][1] * m.m[1][2] * m.m[2][0] + m.m[0][2] * m.m[1][0] * m.m[2][1] - m.m[0][2] * m.m[1][1] * m.m[2][0] - m.m[0][1] * m.m[1][0] * m.m[2][2] -
		m.m[0][0] * m.m[1][2] * m.m[2][1];

	float number;
	number = m.m[0][0] * m.m[1][1] * m.m[2][2] * m.m[3][3] + m.m[0][0] * m.m[1][2] * m.m[2][3] * m.m[3][1] + m.m[0][0] * m.m[1][3] * m.m[2][1] * m.m[3][2] -
		(m.m[0][0] * m.m[1][3] * m.m[2][2] * m.m[3][1]) - (m.m[0][0] * m.m[1][2] * m.m[2][1] * m.m[3][3]) - (m.m[0][0] * m.m[1][1] * m.m[2][3] * m.m[3][2]) -
		(m.m[0][1] * m.m[1][0] * m.m[2][2] * m.m[3][3]) - (m.m[0][2] * m.m[1][0] * m.m[2][3] * m.m[3][1]) - (m.m[0][3] * m.m[1][0] * m.m[2][1] * m.m[3][2]) +
		m.m[0][3] * m.m[1][0] * m.m[2][2] * m.m[3][1] + m.m[0][2] * m.m[1][0] * m.m[2][1] * m.m[3][3] + m.m[0][1] * m.m[1][0] * m.m[2][3] * m.m[3][2] +
		m.m[0][1] * m.m[1][2] * m.m[2][0] * m.m[3][3] + m.m[0][2] * m.m[1][3] * m.m[2][0] * m.m[3][1] + m.m[0][3] * m.m[1][1] * m.m[2][0] * m.m[3][2] -
		(m.m[0][3] * m.m[1][2] * m.m[2][0] * m.m[3][1]) - (m.m[0][2] * m.m[1][1] * m.m[2][0] * m.m[3][3]) - (m.m[0][1] * m.m[1][3] * m.m[2][0] * m.m[3][2]) -
		(m.m[0][1] * m.m[1][2] * m.m[2][3] * m.m[3][0]) - (m.m[0][2] * m.m[1][3] * m.m[2][1] * m.m[3][0]) - (m.m[0][3] * m.m[1][1] * m.m[2][2] * m.m[3][0]) +
		m.m[0][3] * m.m[1][2] * m.m[2][1] * m.m[3][0] + m.m[0][2] * m.m[1][1] * m.m[2][3] * m.m[3][0] + m.m[0][1] * m.m[1][3] * m.m[2][2] * m.m[3][0];

	number = 1.0f / number;

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			result.m[j][i] = number * a.m[j][i];
		}
	}

	return result;
}

Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate) {
	Matrix4x4 S = MakeScaleMatrix(scale);
	Matrix4x4 Rx = MakeRotateXMatrix(rotate.x);
	Matrix4x4 Ry = MakeRotateYMatrix(rotate.y);
	Matrix4x4 Rz = MakeRotateZMatrix(rotate.z);
	Matrix4x4 R = Multiply(Rx, Multiply(Ry, Rz));
	Matrix4x4 T = MakeTranslateMatrix(translate);

	Matrix4x4 W = Multiply(S, Multiply(R, T));

	return W;
}

// 正射影行列4x4
Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip);
// 透視投影行列
Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip);

Vector3 Normalize(const Vector3& v) {
	float length = std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
	return { v.x / length, v.y / length, v.z / length };
}

DirectX::ScratchImage LoadTexture(const std::string& filePath);
Microsoft::WRL::ComPtr<ID3D12Resource> CreateTextureResources(Microsoft::WRL::ComPtr<ID3D12Device> device, const DirectX::TexMetadata& metadata);
void UploadTextureData(Microsoft::WRL::ComPtr<ID3D12Resource> texture, const DirectX::ScratchImage& mipImages);

///////////////////////////////////////////////////////////

// Windowsアプリでのエントリーポイント
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	D3DResourceLeakChecker leakCheck;

	// COMの初期化
	CoInitializeEx(0, COINIT_MULTITHREADED);

	//// 文字を格納する
	//std::string str0{ "STRING!!!" };

	//// 整数を文字列にする
	//std::string str1{ std::to_string(10) };

	//// 出力ウィンドウへの文字出力
	//OutputDebugStringA("Hello,DirectX!\n");





	//=========================================================
	//取得
	WinApp* winApp = nullptr;//WinApp
	winApp = new WinApp();
	Input* input = nullptr;//input
	input = new Input();
	DirectXCommon* dxCommon = nullptr;//dxCommon
	dxCommon = new DirectXCommon();

	//初期化
	winApp->Initialize();
	input->Initialize(winApp);
	dxCommon->Initialize(winApp);
















	//=========================================================
	
	////////////////////////////////////////////////////////////////////////////////////

	//// RootSignature作成
	//D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	//descriptionRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	//// DescriptorRange作成
	//D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	//descriptorRange[0].BaseShaderRegister = 0;                                                   // 0から始まる
	//descriptorRange[0].NumDescriptors = 1;                                                       // 数は1つ
	//descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;                              // SRVを使う
	//descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND; // offsetを自動計算

	//// Samplerの設定
	//D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	//staticSamplers[0].Filter = D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR; // バイリニアフィルタ
	//staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;          // 0~1の範囲外をリピート
	//staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	//staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	//staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;     // 比較しない
	//staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;                       // ありったけのMipmapを使う
	//staticSamplers[0].ShaderRegister = 0;                               // レジスタ番号0を使う
	//staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う
	//descriptionRootSignature.pStaticSamplers = staticSamplers;
	//descriptionRootSignature.NumStaticSamplers = _countof(staticSamplers);

	//// RootParameter作成。複数設定できるので配列。今回は結果ひとつだけなので長さ1の配列
	//D3D12_ROOT_PARAMETER rootParameters[4] = {};
	//rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;    // CBVを使う
	//rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う
	//rootParameters[0].Descriptor.ShaderRegister = 0;                    // レジスタ番号0とバインド

	//rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;     // CBVを使う
	//rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX; // VertexShaderで使う
	//rootParameters[1].Descriptor.ShaderRegister = 0;                     // レジスタ番号0とバインド

	//rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;      // DescriptorTableを使う
	//rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;                // PixelShaderを使う
	//rootParameters[2].DescriptorTable.pDescriptorRanges = descriptorRange;             // Tableの中身の配列を指定
	//rootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange); // Tableで利用する数

	//rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;    // CBVを使う
	//rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う
	//rootParameters[3].Descriptor.ShaderRegister = 1;                    // レジスタ番号1を使う

	//descriptionRootSignature.pParameters = rootParameters;             // ルートパラメータ配列へのポインタ
	//descriptionRootSignature.NumParameters = _countof(rootParameters); // 配列の長さ

	//// WVP用のリソースを作る。
	//Microsoft::WRL::ComPtr<ID3D12Resource> wvpResource = CreateBufferResource(device, sizeof(TransformationMatrix));
	//// データを書き込む
	//TransformationMatrix* wvpData = nullptr;
	//// 書き込むためのアドレスを取得
	//wvpResource->Map(0, nullptr, reinterpret_cast<void**>(&wvpData));
	//// 単位行列を書き込んでおく
	//wvpData->world = MakeIdentity();
	//wvpData->WVP = MakeIdentity();

	//// WVP用のリソースを作る。
	//Microsoft::WRL::ComPtr<ID3D12Resource> directionalLightResource = CreateBufferResource(device, sizeof(DirectionalLight));
	//// データを書き込む
	//DirectionalLight* directionalLightData = nullptr;
	//// 書き込むためのアドレスを取得
	//directionalLightResource->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightData));
	//// デフォルト値
	//directionalLightData->color = { 1.0f, 1.0f, 1.0f, 1.0f };
	//directionalLightData->direction = { 0.0f, -1.0f, 0.0f };
	//directionalLightData->intensity = 1.0f;

	//// シリアライズしてバイナリにする
	//ID3DBlob* signatureBlob = nullptr;
	//ID3DBlob* errorBlob = nullptr;
	//hr = D3D12SerializeRootSignature(&descriptionRootSignature, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	//if (FAILED(hr)) {
	//	Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
	//	assert(false);
	//}
	//// バイナリを元に生成
	//Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature = nullptr;
	//hr = device->CreateRootSignature(0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature));
	//assert(SUCCEEDED(hr));

	//// InputLayoutの設定
	//D3D12_INPUT_ELEMENT_DESC inputElementDescs[3] = {};
	//inputElementDescs[0].SemanticName = "POSITION";
	//inputElementDescs[0].SemanticIndex = 0;
	//inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	//inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	//inputElementDescs[1].SemanticName = "TEXCOORD";
	//inputElementDescs[1].SemanticIndex = 0;
	//inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	//inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	//inputElementDescs[2].SemanticName = "NORMAL";
	//inputElementDescs[2].SemanticIndex = 0;
	//inputElementDescs[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	//inputElementDescs[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	//D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	//inputLayoutDesc.pInputElementDescs = inputElementDescs;
	//inputLayoutDesc.NumElements = _countof(inputElementDescs);

	//// BlendStateの設定
	//D3D12_BLEND_DESC blendDesc{};
	//// 全ての色要素を書き込む
	//blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	//// RasterizerStateの設定
	//D3D12_RASTERIZER_DESC rasterizerDesc{};
	//// 裏面(時計回り)を表示しない
	//rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	//// 三角形の中を塗りつぶす
	//rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

	//// Shaderをコンパイルする
	//IDxcBlob* vertexShaderBlob = CompileShader(L"./Resources/shaders/Object3d.VS.hlsl", L"vs_6_0", dxcUtils, dxcCompiler, includeHandler);
	//assert(vertexShaderBlob != nullptr);

	//IDxcBlob* pixelShaderBlob = CompileShader(L"./Resources/shaders/Object3d.PS.hlsl", L"ps_6_0", dxcUtils, dxcCompiler, includeHandler);
	//assert(pixelShaderBlob != nullptr);

	//// DepthStencilStateの設定
	//D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	//// Depthの機能を有効化
	//depthStencilDesc.DepthEnable = true;
	//// 書き込み
	//depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	//// 比較関数はLessEqual。近ければ描画される
	//depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

	//// PSOを生成
	//D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	//graphicsPipelineStateDesc.pRootSignature = rootSignature.Get();                                           // RootSignature
	//graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;                                                  // inputLayout
	//graphicsPipelineStateDesc.VS = { vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize() }; // VertexShader
	//graphicsPipelineStateDesc.PS = { pixelShaderBlob->GetBufferPointer(), pixelShaderBlob->GetBufferSize() };   // PixelShader
	//graphicsPipelineStateDesc.BlendState = blendDesc;                                                         // BlendDesc
	//graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;                                               // RasterizerDesc
	//// 書き込むRTV情報
	//graphicsPipelineStateDesc.NumRenderTargets = 1;
	//graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	//// 利用するトポロジ(形状)のタイプ。三角形
	//graphicsPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	//// DepthStencilの設定
	//graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
	//graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	//// どのように画面に色を打ち込むかの設定(気にしなくてよい)
	//graphicsPipelineStateDesc.SampleDesc.Count = 1;
	//graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	//// 実際に生成
	//Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState = nullptr;
	//hr = device->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&graphicsPipelineState));
	//assert(SUCCEEDED(hr));

	//// マテリアル用のリソースを作る。今回はcolor1つ分のサイズを用意する
	//Microsoft::WRL::ComPtr<ID3D12Resource> materialResource = CreateBufferResource(device, sizeof(Material));
	//// マテリアルにデータを書き込む
	//Material* materialData = nullptr;
	//// 書き込むためのアドレスを取得
	//materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
	//// 色を書き込む
	//materialData->color = { 1.0f, 1.0f, 1.0f, 1.0f };
	//// 単位行列で初期化
	//materialData->uvTransform = MakeIdentity();

	//// Sprite用の頂点リソースを作る
	//Microsoft::WRL::ComPtr<ID3D12Resource> indexResource = CreateBufferResource(device, sizeof(uint32_t) * 6);

	//// index用バッファビューを作成する
	//D3D12_INDEX_BUFFER_VIEW indexBufferView{};
	//// リソースの先頭のアドレスから使う
	//indexBufferView.BufferLocation = indexResource->GetGPUVirtualAddress();
	//// 使用するリソースはインデックス6つ分のサイズ
	//indexBufferView.SizeInBytes = sizeof(uint32_t) * 6;
	//// インデックスはuint32_tとする
	//indexBufferView.Format = DXGI_FORMAT_R32_UINT;

	//
	//// Transform変数を作る
	//Transform transform{
	//	{1.0f, 1.0f, 1.0f},
	//	{0.0f, 0.0f, 0.0f},
	//	{0.0f, 0.0f, 0.0f}
	//};
	//Transform cameraTransform{
	//	{1.0f, 1.0f, 1.0f  },
	//	{0.0f, 0.0f, 0.0f  },
	//	{0.0f, 0.0f, -10.0f},
	//};

	//
	////----- obj -----
	//// モデルの読み込み 
	//ModelData modelData = LoadObjFile("Resources/models", "axis.obj");
	//// 頂点リソースを作る
	//Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource = CreateBufferResource(device, sizeof(VertexData) * modelData.vertices.size());
	//// 頂点バッファビューを作成する
	//D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	//vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();            // リソースの先頭アドレスから使う
	//vertexBufferView.SizeInBytes = UINT(sizeof(VertexData) * modelData.vertices.size()); // 使用するリソースのサイズは頂点のサイズ
	//vertexBufferView.StrideInBytes = sizeof(VertexData);                                 // 1頂点当たりのサイズ

	//// 頂点リソースにデータを書き込む
	//VertexData* vertexData = nullptr;
	//vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData)); // 書き込むためのアドレスを取得
	//std::memcpy(vertexData, modelData.vertices.data(), sizeof(VertexData) * modelData.vertices.size());

	//// ----- Sprite -----
	//// Sprite用の頂点リソースを作る
	//Microsoft::WRL::ComPtr<ID3D12Resource> vertexResourceSprite = CreateBufferResource(device, sizeof(VertexData) * 6);

	//// 頂点バッファビューを作成する
	//D3D12_VERTEX_BUFFER_VIEW vertexBufferViewSprite{};
	//// リソースの先頭のアドレスから使う
	//vertexBufferViewSprite.BufferLocation = vertexResourceSprite->GetGPUVirtualAddress();
	//// 使用するリソースは頂点6つ分のサイズ
	//vertexBufferViewSprite.SizeInBytes = sizeof(VertexData) * 6;
	//// 頂点あたりのサイズ
	//vertexBufferViewSprite.StrideInBytes = sizeof(VertexData);

	//// indexSprite
	//// 頂点リソースを作る
	//Microsoft::WRL::ComPtr<ID3D12Resource> indexResourceSprite = CreateBufferResource(device, sizeof(uint32_t) * 6);

	//// index用バッファビューを作成する
	//D3D12_INDEX_BUFFER_VIEW indexBufferViewSprite{};
	//// リソースの先頭のアドレスから使う
	//indexBufferViewSprite.BufferLocation = indexResourceSprite->GetGPUVirtualAddress();
	//// 使用するリソースはインデックス6つ分のサイズ
	//indexBufferViewSprite.SizeInBytes = sizeof(uint32_t) * 6;
	//// インデックスはuint32_tとする
	//indexBufferViewSprite.Format = DXGI_FORMAT_R32_UINT;

	//// 頂点リソースにデータを書き込む
	//VertexData* vertexDataSprite = nullptr;
	//// 書き込むためのアドレスを取得
	//vertexResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&vertexDataSprite));
	//// 1枚目の三角形
	//vertexDataSprite[0].position = { 0.0f, 360.0f, 0.0f, 1.0f }; // 左下
	//vertexDataSprite[0].texcoord = { 0.0f, 1.0f };
	//vertexDataSprite[1].position = { 0.0f, 0.0f, 0.0f, 1.0f }; // 左上
	//vertexDataSprite[1].texcoord = { 0.0f, 0.0f };
	//vertexDataSprite[2].position = { 640.0f, 360.0f, 0.0f, 1.0f }; // 右下
	//vertexDataSprite[2].texcoord = { 1.0f, 1.0f };
	//// 2枚目の三角形
	//vertexDataSprite[3].position = { 640.0f, 0.0f, 0.0f, 1.0f }; // 右上
	//vertexDataSprite[3].texcoord = { 1.0f, 0.0f };

	//// インデックスリソースにデータを書き込む
	//uint32_t* indexDataSprite = nullptr;
	//indexResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&indexDataSprite));
	//indexDataSprite[0] = 0;
	//indexDataSprite[1] = 1;
	//indexDataSprite[2] = 2;
	//indexDataSprite[3] = 1;
	//indexDataSprite[4] = 3;
	//indexDataSprite[5] = 2;

	//// Sprite用のマテリアルリソースを作る
	//Microsoft::WRL::ComPtr<ID3D12Resource> materialResourceSprite = CreateBufferResource(device, sizeof(Material));
	//Material* materialDataSprite = nullptr;
	//// Mapしてデータを書き込む
	//materialResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&materialDataSprite));
	//// Lightingを有効にする
	//materialDataSprite->enableLighting = false;
	//// 色を書き込む
	//materialDataSprite->color = { 1.0f, 1.0f, 1.0f, 1.0f };
	//// 単位行列で初期化
	//materialDataSprite->uvTransform = MakeIdentity();

	//// Sprite用のTransformationMatrix用のリソースを作る
	//Microsoft::WRL::ComPtr<ID3D12Resource> transformationMatrixResourceSprite = CreateBufferResource(device, sizeof(TransformationMatrix));
	//// データを書き込む
	//TransformationMatrix* transformationMatrixDataSprite = nullptr;
	//// 書き込むためのアドレスを取得
	//transformationMatrixResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixDataSprite));
	//// 単位行列を書き込んでおく
	//transformationMatrixDataSprite->world = MakeIdentity();
	//transformationMatrixDataSprite->WVP = MakeIdentity();

	//// Transform変数を作る
	//Transform transformSprite{
	//	{1.0f, 1.0f, 1.0f},
	//	{0.0f, 0.0f, 0.0f},
	//	{0.0f, 0.0f, 0.0f}
	//};

	//// Textureを読んで転送する
	//DirectX::ScratchImage mipImages = LoadTexture("Resources/Images/uvChecker.png");
	//const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
	//Microsoft::WRL::ComPtr<ID3D12Resource> textureResource = CreateTextureResources(device, metadata);
	//UploadTextureData(textureResource, mipImages);

	//// 2枚目のTextureを読んで転送する
	//DirectX::ScratchImage mipImages2 = LoadTexture(modelData.material.textureFilePath);
	//const DirectX::TexMetadata& metadata2 = mipImages2.GetMetadata();
	//Microsoft::WRL::ComPtr<ID3D12Resource> textureResource2 = CreateTextureResources(device, metadata2);
	//UploadTextureData(textureResource2, mipImages2);

	////////////////////////////////////////////////////////////////////////////////////////////////

	//// metaDataを基にSRVの設定
	//D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	//srvDesc.Format = metadata.format;
	//srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	//srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	//srvDesc.Texture2D.MipLevels = UINT(metadata.mipLevels);
	//// metaDataを基にSRVの設定
	//D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc2{};
	//srvDesc2.Format = metadata2.format;
	//srvDesc2.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	//srvDesc2.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	//srvDesc2.Texture2D.MipLevels = UINT(metadata2.mipLevels);

	//// SRVを作成するDescriptorHeapの場所を決める
	//D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU = GetCPUDescriptorHandle(srvDescriptorHeap, descriptorSizeSRV, 1);
	//D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU = GetGPUDescriptorHandle(srvDescriptorHeap, descriptorSizeSRV, 1);
	//// SRVを作成するDescriptorHeapの場所を決める
	//D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU2 = GetCPUDescriptorHandle(srvDescriptorHeap, descriptorSizeSRV, 2);
	//D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU2 = GetGPUDescriptorHandle(srvDescriptorHeap, descriptorSizeSRV, 2);

	//// SRVの生成
	//device->CreateShaderResourceView(textureResource.Get(), &srvDesc, textureSrvHandleCPU);
	//device->CreateShaderResourceView(textureResource2.Get(), &srvDesc2, textureSrvHandleCPU2);

	//bool useMonsterBall = true;

	// ウィンドウのxボタンが押されるまでループ
	while (true) { // ゲームループ！！！！！！！！！！
		// Windowにメッセージがきてたら最優先で処理させる
		if (winApp->ProcessMessage()) {
			//ゲームループを抜ける
			break;
		}
		else {

			// ゲームの処理
			//=========================================================
			//入力の更新
			input->Update();






















			//========================================================= 
			//// これから書き込むバックバッファのインデックスを取得
			//UINT backBufferIndex = swapChain->GetCurrentBackBufferIndex();

			//ImGui_ImplDX12_NewFrame();
			//ImGui_ImplWin32_NewFrame();
			//ImGui::NewFrame();

			//// 開発用UIの処理
			//ImGui::Begin("Window");
			//if (ImGui::TreeNode("Camera")) {
			//	ImGui::DragFloat3("CameraTranslate", &cameraTransform.translate.x, 0.01f);
			//	ImGui::DragFloat("CameraRotateX", &cameraTransform.rotate.x, 0.01f);
			//	ImGui::DragFloat("CameraRotateY", &cameraTransform.rotate.y, 0.01f);
			//	ImGui::DragFloat("CameraRotateZ", &cameraTransform.rotate.z, 0.01f);
			//	ImGui::TreePop();
			//}
			//if (ImGui::TreeNode("Light")) {
			//	ImGui::ColorEdit4("color", &materialData->color.x);
			//	ImGui::Checkbox("enableLighting", &materialData->enableLighting);
			//	ImGui::DragFloat3("LightDirection", &directionalLightData->direction.x, 0.01f);
			//	directionalLightData->direction = Normalize(directionalLightData->direction);
			//	ImGui::DragFloat("Intensity", &directionalLightData->intensity, 0.01f);
			//	ImGui::ColorEdit3("LightColor", &directionalLightData->color.x);
			//	ImGui::TreePop();
			//}

			//// ImGui::DragFloat3("translateSprite", &transformSprite.translate.x, 0.1f);
			//// ImGui::ColorEdit4("colorSprite", &materialDataSprite->color.x);

			//if (ImGui::TreeNode("UV")) {
			//	ImGui::DragFloat3("UVTranslate", &uvTransformSprite.translate.x, 0.01f, -10.0f, 10.0f);
			//	ImGui::DragFloat3("UVScale", &uvTransformSprite.scale.x, 0.01f, -10.0f, 10.0f);
			//	ImGui::SliderAngle("UVRotate", &uvTransformSprite.rotate.z);
			//	ImGui::TreePop();
			//}
			//if (ImGui::TreeNode("Object")) {
			//	ImGui::Checkbox("useMonsterBall", &useMonsterBall);
			//	ImGui::DragFloat3("Translate", &transform.translate.x, 0.01f);
			//	ImGui::DragFloat3("Rotate", &transform.rotate.x, 0.01f);
			//	ImGui::DragFloat3("Scale", &transform.scale.x, 0.01f);
			//	ImGui::ColorEdit3("color", &materialData->color.x);
			//	ImGui::TreePop();
			//}

			//ImGui::End();

			////=============================
			////キーボード
			//if (input->PushKey(DIK_0)) {
			//	OutputDebugStringA("Push Hit 0\n");
			//}
			//if (input->TriggerKey(DIK_1)) {
			//	OutputDebugStringA("Trigger Hit 0\n");
			//}
			////=============================

			//// TransitionBarrierの設定
			//D3D12_RESOURCE_BARRIER barrier{};
			//// 今回のバリアはTransition
			//barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			//// Noneにしておく
			//barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
			//// バリアを張る対象のリソース。現在のバックバッファに対して行う
			//barrier.Transition.pResource = swapChainResources[backBufferIndex].Get();
			//// 遷移前(現在)のResourceState
			//barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
			//// 遷移後のResourceState
			//barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
			//// TransitionBarrierを張る
			//commandList->ResourceBarrier(1, &barrier);

			//// 描画先のRTVとDSVを設定する
			//D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
			//commandList->OMSetRenderTargets(1, &rtvHandles[backBufferIndex], false, &dsvHandle);

			//// 指定した深度で画面全体をクリアする
			//commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
			//// 指定した色で画面全体をクリアする
			//float clearColor[] = { 0.1f, 0.25f, 0.5f, 1.0f }; // 青っぽい色。RGBAの順
			//commandList->ClearRenderTargetView(rtvHandles[backBufferIndex], clearColor, 0, nullptr);

			//// 描画用DescriptorHeap設定
			//Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeaps[] = { srvDescriptorHeap };
			//commandList->SetDescriptorHeaps(1, descriptorHeaps->GetAddressOf());

			//// ImGuiの内部コマンドを生成する
			//ImGui::Render();

			///////////////////////////////////////////////////////////////////////////////////////

			//commandList->RSSetViewports(1, &viewport);       // ViewPortを設定
			//commandList->RSSetScissorRects(1, &scissorRect); // Scissorを設定
			//// RootSignatureを設定。PSOに設定しているけど別途設定が必要
			//commandList->SetGraphicsRootSignature(rootSignature.Get());
			//commandList->SetPipelineState(graphicsPipelineState.Get());
			//commandList->IASetVertexBuffers(0, 1, &vertexBufferView); // VBVを設定
			//// 形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけばよい
			//commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			//// マテリアルCBufferの場所を設定
			//commandList->SetGraphicsRootConstantBufferView(0, materialResource->GetGPUVirtualAddress());

			//commandList->IASetIndexBuffer(&indexBufferView);

			//// wvp用のCBufferの場所を設定
			//commandList->SetGraphicsRootConstantBufferView(1, wvpResource->GetGPUVirtualAddress());

			//commandList->SetGraphicsRootDescriptorTable(2, useMonsterBall ? textureSrvHandleGPU2 : textureSrvHandleGPU);
			//commandList->SetGraphicsRootConstantBufferView(3, directionalLightResource->GetGPUVirtualAddress());

			//commandList->DrawInstanced(UINT(modelData.vertices.size()), 1, 0, 0);

			//// 三角形
			//// transform.rotate.y += 0.03f;
			//Matrix4x4 worldMatrix = MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);

			//Matrix4x4 cameraMatrix = MakeAffineMatrix(cameraTransform.scale, cameraTransform.rotate, cameraTransform.translate);
			//Matrix4x4 viewMatrix = Inverse(cameraMatrix);
			//Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(WinApp::kClientWidth) / float(WinApp::kClientHeight), 0.1f, 100.0f);
			//// WVPMatrixを作る
			//Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(viewMatrix, projectionMatrix));
			//wvpData->world = worldViewProjectionMatrix;
			//wvpData->WVP = worldViewProjectionMatrix;

			//// 描画！(DrawCall/ドローコール)。3頂点で1つのインスタンス。
			//commandList->DrawIndexedInstanced(6, 1, 0, 0, 0);

			//// Sprite
			//commandList->SetGraphicsRootDescriptorTable(2, textureSrvHandleGPU);

			//// Sprite用のWorldViewProjectionMatrixを作る
			//Matrix4x4 worldMatrixSprite = MakeAffineMatrix(transformSprite.scale, transformSprite.rotate, transformSprite.translate);
			//Matrix4x4 viewMatrixSprite = MakeIdentity();
			//Matrix4x4 projectionMatrixSprite = MakeOrthographicMatrix(0.0f, 0.0f, float(WinApp::kClientWidth), float(WinApp::kClientHeight), 0.0f, 100.0f);
			//Matrix4x4 worldViewProjectionMatrixSprite = Multiply(worldMatrixSprite, Multiply(viewMatrixSprite, projectionMatrixSprite));
			//transformationMatrixDataSprite->world = worldViewProjectionMatrixSprite;
			//transformationMatrixDataSprite->WVP = worldViewProjectionMatrixSprite;
			//// マテリアルCBufferの場所を設定
			//commandList->SetGraphicsRootConstantBufferView(0, materialResourceSprite->GetGPUVirtualAddress());
			//// Spriteの描画
			//commandList->IASetIndexBuffer(&indexBufferViewSprite);          // IBVを設定
			//commandList->IASetVertexBuffers(0, 1, &vertexBufferViewSprite); // VBVを設定
			//// TransformMatrixCBufferの場所を設定
			//commandList->SetGraphicsRootConstantBufferView(1, transformationMatrixResourceSprite->GetGPUVirtualAddress());
			//// 描画
			//// commandList->DrawIndexedInstanced(6, 1, 0, 0, 0);

			//// ImGuiでパラメータ編集を可能にさせる
			//Matrix4x4 uvTransformMatrix = MakeScaleMatrix(uvTransformSprite.scale);
			//uvTransformMatrix = Multiply(uvTransformMatrix, MakeRotateZMatrix(uvTransformSprite.rotate.z));
			//uvTransformMatrix = Multiply(uvTransformMatrix, MakeTranslateMatrix(uvTransformSprite.translate));
			//materialDataSprite->uvTransform = uvTransformMatrix;

			///////////////////////////////////////////////////////////////////////////////////////

			//// 実際のcommandListのImGuiの描画コマンドを積む
			//ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList.Get());

			///////////////////////////////////////////////////////////////////////////////////////

			//// 画面に描く処理は全て終わり、画面に映すので状態を遷移
			//// 今回はRenderTargetからPresentにする
			//barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
			//barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
			//// TransitionBarrierを振る
			//commandList->ResourceBarrier(1, &barrier);

			//// コマンドリストの内容を確定させる。すべてのコマンドを詰んでからCloseすること
			//hr = commandList->Close();
			//assert(SUCCEEDED(hr));

			//// GPUにコマンドリストの実行を行わせる
			//Microsoft::WRL::ComPtr<ID3D12CommandList> commandLists[] = { commandList };
			//commandQueue->ExecuteCommandLists(1, commandLists->GetAddressOf());
			//// GPUとOSに画面の交換を行うように通知する
			//swapChain->Present(1, 0); // 面倒くさいやつ

			//// Fenceの値を更新
			//fenceValue++;
			//// GPUがここまでたどり着いたときに、Fenceの値を指定した値に代入するようにSignalを送る
			//commandQueue->Signal(fence.Get(), fenceValue);

			//// Fenceの値が確定したSignal値にたどりついているか確認する
			//// GetCompletedValueの初期値はFence作成時に渡した初期値
			//if (fence->GetCompletedValue() < fenceValue) {
			//	// 指定したSignalにたどり着いていないので、たどりつくまで待つようにイベントを設定する
			//	fence->SetEventOnCompletion(fenceValue, fenceEvent);
			//	// イベントをまつ
			//	WaitForSingleObject(fenceEvent, INFINITE);
			//}

			//// 次のフレーム用のコマンドリストを準備
			//hr = commandAllocator->Reset();
			//assert(SUCCEEDED(hr));
			//hr = commandList->Reset(commandAllocator.Get(), nullptr);
			//assert(SUCCEEDED(hr));
		}
	}

	//// 解放処理
	//signatureBlob->Release();	//if (errorBlob) {
	//	errorBlob->Release();
	//}
	//pixelShaderBlob->Release();
	//vertexShaderBlob->Release();

	//CloseHandle(fenceEvent);


	//////////////////////////////////////////////

	// ImGuiの終了処理
	// 初期化と逆順に行う
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	winApp->Finalize();

	//=================================================
	delete input;
	delete winApp;
	delete dxCommon;













	//=================================================

	return 0;
}

// 関数の中身
//void Log(const std::string& message) { OutputDebugStringA(message.c_str()); }

//std::wstring ConvertString(const std::string& str) {
//	if (str.empty()) {
//		return std::wstring();
//	}
//
//	auto sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(&str[0]), static_cast<int>(str.size()), NULL, 0);
//	if (sizeNeeded == 0) {
//		return std::wstring();
//	}
//	std::wstring result(sizeNeeded, 0);
//	MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(&str[0]), static_cast<int>(str.size()), &result[0], sizeNeeded);
//	return result;
//}
//
//std::string ConvertString(const std::wstring& str) {
//	if (str.empty()) {
//		return std::string();
//	}
//
//	auto sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), NULL, 0, NULL, NULL);
//	if (sizeNeeded == 0) {
//		return std::string();
//	}
//	std::string result(sizeNeeded, 0);
//	WideCharToMultiByte(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), result.data(), sizeNeeded, NULL, NULL);
//	return result;
//}

//IDxcBlob* CompileShader(
//	// CompilerするShaderファイルへのパス
//	const std::wstring& filePath,
//	// Compilerに使用するProfile
//	const wchar_t* profile,
//	// 初期化で生成したもの3つ
//	IDxcUtils* dxcUtils, IDxcCompiler3* dxcCompiler, IDxcIncludeHandler* includeHandler) {
//
//	// これからシェーダーをコンパイルする旨をログにだす
//	Log(ConvertString(std::format(L"Begin CompileShader, path:{}, profile{}\n", filePath, profile)));
//	// hlslファイルを読む
//	IDxcBlobEncoding* shaderSource = nullptr;
//	HRESULT hr = dxcUtils->LoadFile(filePath.c_str(), nullptr, &shaderSource);
//	// 読めなかったらとめる
//	assert(SUCCEEDED(hr));
//	// 読み込んだファイル内容を設定する
//	DxcBuffer shaderSourceBuffer;
//	shaderSourceBuffer.Ptr = shaderSource->GetBufferPointer();
//	shaderSourceBuffer.Size = shaderSource->GetBufferSize();
//	shaderSourceBuffer.Encoding = DXC_CP_UTF8; // UTF8の文字コードであることを通知
//
//	LPCWSTR arguments[] = {
//		filePath.c_str(), // コンパイル対象のhlslファイル名
//		L"-E",
//		L"main", // エントリーポイントの指定。基本的にmain以外にはしない
//		L"-T",
//		profile, // ShaderProfileの設定
//		L"-Zi",
//		L"-Qembed_debug", // デバッグ用の情報を埋め込む
//		L"-Od",           // 最適化を外しておく
//		L"-Zpr",          // メモリレイアウトは行優先
//	};
//	// 実際にShaderをコンパイルする
//	IDxcResult* shaderResult = nullptr;
//	hr = dxcCompiler->Compile(
//		&shaderSourceBuffer,        // 読み込んだファイル
//		arguments,                  // コンパイルオプション
//		_countof(arguments),        // コンパイルオプションの数
//		includeHandler,             // includeが含まれた諸々
//		IID_PPV_ARGS(&shaderResult) // コンパイル結果
//	);
//	// コンパイルエラーではなくdxcが起動できないなど致命的な状況
//	assert(SUCCEEDED(hr));
//
//	// 警告・エラーがでていたらログに出してとめる
//	IDxcBlobUtf8* shaderError = nullptr;
//	shaderResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&shaderError), nullptr);
//	if (shaderError != nullptr && shaderError->GetStringLength() != 0) {
//		Log(shaderError->GetStringPointer());
//		// 警告・エラーダメゼッタイ
//		assert(false);
//	}
//
//	// コンパイル結果から実行用のバイナリ部分を取得
//	IDxcBlob* shaderBlob = nullptr;
//	hr = shaderResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderBlob), nullptr);
//	assert(SUCCEEDED(hr));
//	// 成功したログを出す
//	Log(ConvertString(std::format(L"Compile Succeeded, path:{}, profile{}\n", filePath, profile)));
//	// もう使わないリソースを解放
//	shaderSource->Release();
//	shaderResult->Release();
//	// 実行用のバイナリを返却
//	return shaderBlob;
//}
//
//Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(Microsoft::WRL::ComPtr<ID3D12Device> device, size_t sizeInBytes) {
//
//	// リソース用のヒープの設定
//	D3D12_HEAP_PROPERTIES uploadHeapProperties{};
//	uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD; // UploadHeapを使う
//	// リソースの設定
//	D3D12_RESOURCE_DESC resourceDesc{};
//	// バッファリソース。テクスチャの場合はまた別の設定をする
//	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
//	resourceDesc.Width = sizeInBytes; // リソースのサイズ。今回はVector4を3頂点分
//	// バッファの場合はこれらは1にする決まり
//	resourceDesc.Height = 1;
//	resourceDesc.DepthOrArraySize = 1;
//	resourceDesc.MipLevels = 1;
//	resourceDesc.SampleDesc.Count = 1;
//	// バッファの場合はこれにする決まり
//	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
//	// 実際にリソースを作る
//	Microsoft::WRL::ComPtr<ID3D12Resource> resource = nullptr;
//	HRESULT hr = device->CreateCommittedResource(&uploadHeapProperties, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&resource));
//	assert(SUCCEEDED(hr));
//
//	return resource;
//}

//MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename) {
//	MaterialData materialData;
//	std::string line;
//	std::ifstream file(directoryPath + "/" + filename);
//	assert(file.is_open());
//
//	while (std::getline(file, line)) {
//		std::string identifier;
//		std::istringstream s(line);
//		s >> identifier;
//
//		if (identifier == "newmtl") {
//			s >> materialData.name;
//		}
//		else if (identifier == "Ns") {
//			s >> materialData.Ns;
//		}
//		else if (identifier == "Ka") {
//			s >> materialData.Ka.r >> materialData.Ka.g >> materialData.Ka.b;
//		}
//		else if (identifier == "Kd") {
//			s >> materialData.Kd.r >> materialData.Kd.g >> materialData.Kd.b;
//		}
//		else if (identifier == "Ks") {
//			s >> materialData.Ks.r >> materialData.Ks.g >> materialData.Ks.b;
//		}
//		else if (identifier == "Ni") {
//			s >> materialData.Ni;
//		}
//		else if (identifier == "d") {
//			s >> materialData.d;
//		}
//		else if (identifier == "illum") {
//			s >> materialData.illum;
//		}
//		else if (identifier == "map_Kd") {
//			std::string textureFilename;
//			s >> textureFilename;
//			materialData.textureFilePath = directoryPath + "/" + textureFilename;
//		}
//	}
//	return materialData;
//}
//
//ModelData LoadObjFile(const std::string& directoryPath, const std::string& filename) {
//	ModelData modelData;
//	std::vector<Vector4> positions;
//	std::vector<Vector3> normals;
//	std::vector<Vector2> texcoords;
//	std::string line;
//
//	std::ifstream file(directoryPath + "/" + filename);
//	assert(file.is_open());
//
//	while (std::getline(file, line)) {
//		std::string identifier;
//		std::istringstream s(line);
//		s >> identifier;
//
//		if (identifier == "v") {
//			Vector4 position;
//			s >> position.x >> position.y >> position.z;
//			position.x *= -1.0f;
//			position.w = 1.0f;
//			positions.push_back(position);
//		}
//		else if (identifier == "vt") {
//			Vector2 texcoord;
//			s >> texcoord.x >> texcoord.y;
//			texcoord.y = 1.0f - texcoord.y;
//			texcoords.push_back(texcoord);
//		}
//		else if (identifier == "vn") {
//			Vector3 normal;
//			s >> normal.x >> normal.y >> normal.z;
//			normal.x *= -1.0f;
//			normals.push_back(normal);
//		}
//		else if (identifier == "f") {
//			VertexData triangle[3];
//			for (int32_t faceVertex = 0; faceVertex < 3; ++faceVertex) {
//				std::string vertexDefinition;
//				s >> vertexDefinition;
//				std::istringstream v(vertexDefinition);
//				uint32_t elementIndices[3];
//				for (int32_t element = 0; element < 3; ++element) {
//					std::string index;
//					std::getline(v, index, '/');
//					elementIndices[element] = std::stoi(index);
//				}
//				Vector4 position = positions[elementIndices[0] - 1];
//				Vector2 texcoord = texcoords[elementIndices[1] - 1];
//				Vector3 normal = normals[elementIndices[2] - 1];
//				VertexData vertex = { position, texcoord, normal };
//				triangle[faceVertex] = vertex;
//			}
//			modelData.vertices.push_back(triangle[2]);
//			modelData.vertices.push_back(triangle[1]);
//			modelData.vertices.push_back(triangle[0]);
//		}
//		else if (identifier == "mtllib") {
//			std::string materialFilename;
//			s >> materialFilename;
//			modelData.material = LoadMaterialTemplateFile(directoryPath, materialFilename);
//		}
//	}
//
//	return modelData;
//}
//
//DirectX::ScratchImage LoadTexture(const std::string& filePath) {
//	// テクスチャファイルを読んでプログラムで扱えるようにする
//	DirectX::ScratchImage image{};
//	std::wstring filePathW = ConvertString(filePath);
//	HRESULT hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
//	assert(SUCCEEDED(hr));
//
//	// ミニマップの作成
//	DirectX::ScratchImage mipImages{};
//	hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 0, mipImages);
//	assert(SUCCEEDED(hr));
//
//	// ミニマップ付きのデータを返す
//	return mipImages;
//}
//
//Microsoft::WRL::ComPtr<ID3D12Resource> CreateTextureResources(Microsoft::WRL::ComPtr<ID3D12Device> device, const DirectX::TexMetadata& metadata) {
//	// metadataを基にResourcesの設定
//	D3D12_RESOURCE_DESC resourceDesc{};
//	resourceDesc.Width = UINT(metadata.width);                             // Textureの幅
//	resourceDesc.Height = UINT(metadata.height);                           // Textureの高さ
//	resourceDesc.MipLevels = UINT16(metadata.mipLevels);                   // mipmapの数
//	resourceDesc.DepthOrArraySize = UINT16(metadata.arraySize);            // 奥行き or 配列Textureの配列数
//	resourceDesc.Format = metadata.format;                                 ////TextureのFormat
//	resourceDesc.SampleDesc.Count = 1;                                     // サンプリングカウント。1固定。
//	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION(metadata.dimension); // Textureの次元数。普段使ってるのは2次元
//
//	// 利用するHeapの設定。非常に特殊な運用。
//	D3D12_HEAP_PROPERTIES heapProperties{};
//	heapProperties.Type = D3D12_HEAP_TYPE_CUSTOM;                        // 細かい設定を行う
//	heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK; // WriteBackポリシーでCPUアクセス可能
//	heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;          // プロセッサの近くに配置
//
//	// Resourcesの生成
//	Microsoft::WRL::ComPtr<ID3D12Resource> resource = nullptr;
//	HRESULT hr = device->CreateCommittedResource(
//		&heapProperties,                   // Heapの設定
//		D3D12_HEAP_FLAG_NONE,              // Heapの特殊な設定
//		&resourceDesc,                     // Resource設定
//		D3D12_RESOURCE_STATE_GENERIC_READ, // 初回のResourceState。Textureは読むだけ
//		nullptr,                           // Clear最適値。使わないのでnullptr
//		IID_PPV_ARGS(&resource)            // 作成するResourceポインタへのポインタ
//	);
//	assert(SUCCEEDED(hr));
//
//	return resource;
//}
//
//void UploadTextureData(Microsoft::WRL::ComPtr<ID3D12Resource> texture, const DirectX::ScratchImage& mipImages) {
//	// Meta情報を取得
//	const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
//	// 全MipMapについて
//	for (size_t mipLevel = 0; mipLevel < metadata.mipLevels; ++mipLevel) {
//		// MipMapLevelを指定して各Imageを取得
//		const DirectX::Image* img = mipImages.GetImage(mipLevel, 0, 0);
//		// Textureに転送
//		HRESULT hr = texture->WriteToSubresource(
//			UINT(mipLevel),       //
//			nullptr,              // 全領域へコピー
//			img->pixels,          // 元データアドレス
//			UINT(img->rowPitch),  // 1ラインサイズ
//			UINT(img->slicePitch) // 1枚サイズ
//		);
//		assert(SUCCEEDED(hr));
//	}
//}
//
//// 正射影行列4x4
//Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip) {
//	Matrix4x4 result;
//
//	result.m[0][0] = 2 / (right - left);
//	result.m[0][1] = 0;
//	result.m[0][2] = 0;
//	result.m[0][3] = 0;
//
//	result.m[1][0] = 0;
//	result.m[1][1] = 2 / (top - bottom);
//	result.m[1][2] = 0;
//	result.m[1][3] = 0;
//
//	result.m[2][0] = 0;
//	result.m[2][1] = 0;
//	result.m[2][2] = 1 / (farClip - nearClip);
//	result.m[2][3] = 0;
//
//	result.m[3][0] = (left + right) / (left - right);
//	result.m[3][1] = (top + bottom) / (bottom - top);
//	result.m[3][2] = nearClip / (nearClip - farClip);
//	result.m[3][3] = 1;
//
//	return result;
//};
//
//// 透視投影行列
//Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip) {
//	Matrix4x4 result;
//
//	result.m[0][0] = (1 / aspectRatio) * (1 / tan(fovY / 2));
//	result.m[0][1] = 0;
//	result.m[0][2] = 0;
//	result.m[0][3] = 0;
//
//	result.m[1][0] = 0;
//	result.m[1][1] = (1 / tan(fovY / 2));
//	result.m[1][2] = 0;
//	result.m[1][3] = 0;
//
//	result.m[2][0] = 0;
//	result.m[2][1] = 0;
//	result.m[2][2] = farClip / (farClip - nearClip);
//	result.m[2][3] = 1;
//
//	result.m[3][0] = 0;
//	result.m[3][1] = 0;
//	result.m[3][2] = -(nearClip * farClip) / (farClip - nearClip);
//	result.m[3][3] = 0;
//
//	return result;
//}

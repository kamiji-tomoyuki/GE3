#pragma once
#include <d3d12.h>
#include <wrl.h>

#include "DirectXCommon.h"

//スプライト共通部
class SpriteCommon
{
public://メンバ関数
	//初期化
	void Initialize(DirectXCommon* dxCommon);

	//共通描画設定
	void PreDraw();

public://ゲッター
	DirectXCommon* GetDxCommon() const { return dxCommon_; }


private:
	DirectXCommon* dxCommon_;

	//ルートシグネチャ
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature = nullptr;
	//グラフィックスパイプラインステート
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	
	//グラフィックスパイプライン
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState = nullptr;


private:
	//ルートシグネチャの作成
	void CreateRootSignature();
	//グラフィックスパイプラインの生成
	void CreateGraphicsPipelineState();

private://PSO生成のための関数
	//シェーダーのコンパイル
	IDxcBlob* CompileShader(
		// CompilerするShaderファイルへのパス
		const std::wstring& filePath,
		// Compilerに使用するProfile
		const wchar_t* profile,
		// 初期化で生成したもの3つ
		IDxcUtils* dxcUtils,
		IDxcCompiler3* dxcCompiler,
		IDxcIncludeHandler* includeHandler);

};


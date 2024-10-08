#pragma once

#include "DirectXCommon.h"

class Object3dCommon
{
public:
	// 初期化
	void Initialize(DirectXCommon* dxCommon);

	// 共通描画設定
	void PreDraw();

public:
	// dxCommonの取得
	DirectXCommon* GetDxCommon() const { return dxCommon_; }

private:
	// ルートシグネチャの作成
	void CreateRootSignature();
	// グラフィックスパイプラインステートの生成
	void CreateGraphicsPipeline();

private:
	DirectXCommon* dxCommon_;

	// --- ルートシグネチャ ---
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature = nullptr;
	// --- グラフィックスパイプライン ---
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState = nullptr;
	
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};

	D3D12_BLEND_DESC blendDesc{};
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};

	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	D3D12_ROOT_PARAMETER rootParameters[4] = {};
	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[3] = {};

	Microsoft::WRL::ComPtr <IDxcBlob> vertexShaderBlob;
	Microsoft::WRL::ComPtr <IDxcBlob> pixelShaderBlob;

};


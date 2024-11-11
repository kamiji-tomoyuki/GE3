#pragma once
#include <d3d12.h>
#include <wrl.h>

#include "DirectXCommon.h"

//スプライト共通部
class SpriteCommon
{
#pragma region シングルトンインスタンス
private:
	static SpriteCommon* instance;

	SpriteCommon() = default;
	~SpriteCommon() = default;
	SpriteCommon(SpriteCommon&) = delete;
	SpriteCommon& operator = (SpriteCommon&) = delete;

public:
	// シングルトンインスタンスの取得
	static SpriteCommon* GetInstance();
	// 終了
	void Finalize();
#pragma endregion シングルトンインスタンス

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
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;
	//グラフィックスパイプラインステート
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	
	//グラフィックスパイプライン
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState;


private:
	//ルートシグネチャの作成
	void CreateRootSignature();
	//グラフィックスパイプラインの生成
	void CreateGraphicsPipelineState();

private://PSO生成のための関数
	
};


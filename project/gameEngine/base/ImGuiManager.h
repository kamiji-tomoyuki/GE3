#pragma once
#include <WinApp.h>
#include <DirectXCommon.h>

// ImGui管理
class ImGuiManager
{
public:
	// 初期化
	void Initialize(WinApp* winApp, DirectXCommon* dxCommon);

	// 終了
	void Finalize();

public:
	// ImGui受付開始
	void Begin();

	// ImGui受付終了
	void End();

	// 画面への描画
	void Draw();

private:
	WinApp* winApp_;
	DirectXCommon* dxCommon_;

	// SRV用デスクリプタヒープ
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvHeap_;

};


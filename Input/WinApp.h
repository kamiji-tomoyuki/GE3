#pragma once
#include <Windows.h>
#include <wrl.h>

#include "../externals/imgui/imgui.h"

class WinApp
{
public://静的メンバ関数
	static LRESULT  CALLBACK WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

public://メンバ関数
	//初期化
	void Initialize();
	//更新
	void Update();

	//getter
	HINSTANCE GetHInstance()const { return wc.hInstance; }
	HWND GetHwnd()const { return hwnd; }

public://定数
	//クライアント領域のサイズ
	static const int32_t kClientWidth = 1280;
	static const int32_t kClientHeight = 720;

private:
	//ウインドウクラスの設定
	WNDCLASS wc{};
	//ウインドウハンドル
	HWND hwnd = nullptr;
};


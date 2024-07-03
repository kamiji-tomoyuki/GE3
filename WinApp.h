#pragma once
#include <Windows.h>
#include <wrl.h>

#include "externals/imgui/imgui.h"

class WinApp
{
public://静的メンバ関数
	static LRESULT  CALLBACK WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

public://メンバ関数
	//初期化
	void Initialize();
	//更新
	void Update();
};


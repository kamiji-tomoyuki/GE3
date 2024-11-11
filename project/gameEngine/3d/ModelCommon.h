#pragma once
#include "DirectXCommon.h"

class ModelCommon
{
public:
	// 初期化
	void Initialize(DirectXCommon* dxCommon);

public:
	// dxCommonの取得
	DirectXCommon* GetDxCommon() const { return dxCommon_; }

private:
	DirectXCommon* dxCommon_;
};


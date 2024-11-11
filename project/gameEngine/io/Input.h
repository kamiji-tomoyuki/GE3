#pragma once
#include <windows.h>
#include <wrl.h>

#define DIRECTINPUT_VERSION 0x0800 //Directinputのバージョン指定

#include "dinput.h"
#include "assert.h"

#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")

#include "WinApp.h"

class Input
{
#pragma region シングルトンインスタンス
private:
	static Input* instance;

	Input() = default;
	~Input() = default;
	Input(Input&) = delete;
	Input& operator = (Input&) = delete;

public:
	// シングルトンインスタンスの取得
	static Input* GetInstance();
	// 終了
	void Finalize();
#pragma endregion シングルトンインスタンス

public:
	//namespace省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public://メンバ関数
	//初期化
	void Initialize(WinApp* winApp);
	//更新
	void Update();

	//キーの押下をチェック
	bool PushKey(BYTE keyNumber);
	//キーのトリガーをチェック
	bool TriggerKey(BYTE keynumber);

private://メンバ変数
	HRESULT result_;

	//DirectInputのインスタンス
	ComPtr<IDirectInput8> directInput_;

	//キーボードのデバイス
	ComPtr <IDirectInputDevice8> keyboard_;

	//キー
	BYTE key_[256] = {};
	//前回のキー
	BYTE keyPre_[256] = {};

	//WindowsAPI
	WinApp* winApp_ = nullptr;
};


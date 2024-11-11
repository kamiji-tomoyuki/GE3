#include "Input.h"

Input* Input::instance = nullptr;

Input* Input::GetInstance()
{
	if (instance == nullptr) {
		instance = new Input;
	}
	return instance;
}

void Input::Finalize()
{
	delete instance;
	instance = nullptr;
}

void Input::Initialize(WinApp* winApp)
{
	//借りてきたWinAppのインスタンスを記録
	this->winApp_ = winApp;

	//DirectInputのインスタンスの生成
	result_ = DirectInput8Create(winApp->GetHInstance(), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&directInput_, nullptr);
	assert(SUCCEEDED(result_));
	//キーボードデバイス生成 
	result_ = directInput_->CreateDevice(GUID_SysKeyboard, &keyboard_, NULL);
	assert(SUCCEEDED(result_));
	//入力データ形式のセット
	result_ = keyboard_->SetDataFormat(&c_dfDIKeyboard);
	assert(SUCCEEDED(result_));
	//排他制御レベルのセット
	result_ = keyboard_->SetCooperativeLevel(winApp->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(result_));
}

void Input::Update()
{
	//前回のキー入力を保存
	memcpy(keyPre_, key_, sizeof(key_));

	//キーボード情報の取得開始
	result_ = keyboard_->Acquire();
	//全キーの入門情報を取得
	result_ = keyboard_->GetDeviceState(sizeof(key_), key_);
}

bool Input::PushKey(BYTE keyNumber)
{
	//指定キーを押していればtrueを返す
	if (key_[keyNumber]) {
		return true;
	}
	//そうでなければfalseを返す
	return false;
}

bool Input::TriggerKey(BYTE keyNumber)
{
	//前回のキーと同じな時はfalseを返す
	if (!key_[keyNumber] && keyPre_[keyNumber]) {
		return true;
	}
	//そうでなければtrueを返す
	return false;
}

#include "core/Input.h"
#include "System.h"


// static変数初期化
Input* Input::instance_ = nullptr;

Input::~Input(){}


Input* Input::GetInstance(){
	if (!instance_){
		instance_ = new Input();
	}

	return instance_;
}

void Input::Initialize(){
	//インスタンスがなければ作る
	GetInstance();

	//directInputの初期化
	instance_->DirectInputInitialize();
}

void Input::Update(){
	//キーボード更新
	instance_->KeyboardUpdate();
}

void Input::Finalize(){
	// キーボードデバイスの解放
	if (instance_->keyboard_){
		instance_->keyboard_->Unacquire(); // デバイスを解放する
		instance_->keyboard_.Reset(); // ComPtrのリセット
	}

	// directInputの解放
	if (instance_->directInput_){
		instance_->directInput_.Reset(); // ComPtrのリセット
	}

	// インスタンスの解放
	delete instance_;
	instance_ = nullptr;
}

void Input::DirectInputInitialize(){
	HRESULT hr;
	hr = DirectInput8Create(
		System::GetHinstance(), DIRECTINPUT_VERSION, IID_IDirectInput8,
		( void** ) &directInput_, nullptr);
	assert(SUCCEEDED(hr));

	//キーボードデバイスの生成
	hr = directInput_->CreateDevice(GUID_SysKeyboard, &keyboard_, NULL);
	assert(SUCCEEDED(hr));

	//入力データ形式のセット
	hr = keyboard_->SetDataFormat(&c_dfDIKeyboard);
	assert(SUCCEEDED(hr));

	//排他制御レベルのセット
	hr = keyboard_->SetCooperativeLevel(
		System::GetHWND(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY
	);
	assert(SUCCEEDED(hr));
}



/////////////////////////////////////////////////////////////////////////////////////////
/*                                     キーボード                                        */
/////////////////////////////////////////////////////////////////////////////////////////

void Input::KeyboardUpdate(){
	keyPre_ = key_;
	// キーボードの入力情報取得
	HRESULT hr = keyboard_->GetDeviceState(sizeof(key_), key_.data());
	if (FAILED(hr)){
		// 失敗の時再取得
		hr = keyboard_->Acquire();
		while (hr == DIERR_INPUTLOST){
			hr = keyboard_->Acquire();
		}
	}
}

bool Input::PushKey(uint32_t keyNum) {
	return instance_->key_[keyNum];
}

bool Input::TriggerKey(uint32_t keyNum) {
	return instance_->key_[keyNum] && !instance_->keyPre_[keyNum];
}


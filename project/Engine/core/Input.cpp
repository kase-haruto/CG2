#include "engine/core/Input.h"
#include "engine/core/System.h"


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

	/*=======================================================================================
				keyboad
	========================================================================================*/
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


	/*=======================================================================================
				mouse
	========================================================================================*/
	//デバイスの生成
	hr = directInput_->CreateDevice(GUID_SysMouse, &mouse_, NULL);
	assert(SUCCEEDED(hr));

	//入力データ形式のセット
	hr = mouse_->SetDataFormat(&c_dfDIMouse);
	assert(SUCCEEDED(hr));

	//排他制御レベルのセット
	hr = mouse_->SetCooperativeLevel(
		System::GetHWND(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE
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

bool Input::PushKey(uint32_t keyNum){
	return instance_->key_[keyNum];
}

bool Input::TriggerKey(uint32_t keyNum){
	return instance_->key_[keyNum] && !instance_->keyPre_[keyNum];
}

/////////////////////////////////////////////////////////////////////////////////////////
/*										 マウス                                        */
/////////////////////////////////////////////////////////////////////////////////////////

void Input::MouseUpdate(){
	mouseStatePre_ = mouseState_;
	HRESULT hr = mouse_->GetDeviceState(sizeof(DIMOUSESTATE), &mouseState_);
	if (FAILED(hr)){
		hr = mouse_->Acquire();
		while (hr == DIERR_INPUTLOST){
			hr = mouse_->Acquire();
		}
	}

	// マウスの座標更新（画面座標にマッピングする処理）
	mousePos_.x += mouseState_.lX;
	mousePos_.y += mouseState_.lY;

	// ホイール量をfloatで更新
	mouseWheel_ = static_cast< float >(mouseState_.lZ) / 120.0f; // 1段階のホイール移動を1.0fにする
}

bool Input::PushMouseButton(int button){
	return instance_->mouseState_.rgbButtons[button] & 0x80;
}

bool Input::TriggerMouseButton(int button){
	return (instance_->mouseState_.rgbButtons[button] & 0x80) &&
		!(instance_->mouseStatePre_.rgbButtons[button] & 0x80);
}

Vector2 Input::GetMousePosition(){
	return instance_->mousePos_;
}

float Input::GetMouseWheel(){
	return instance_->mouseWheel_;
}
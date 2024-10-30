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

	instance_->MouseUpdate();
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
	// 現在のマウスの状態を前回の状態として保存
	mouseStatePre_ = mouseState_;

	// DirectInputのマウスの状態を取得
	HRESULT hr = mouse_->GetDeviceState(sizeof(DIMOUSESTATE), &mouseState_);
	if (FAILED(hr)){
		hr = mouse_->Acquire();
		while (hr == DIERR_INPUTLOST){
			hr = mouse_->Acquire();
		}
	}

	// マウスの相対移動量を加算
	mousePos_.x += mouseState_.lX;
	mousePos_.y += mouseState_.lY;

	// Windows APIを使って絶対的なスクリーン座標を取得
	POINT cursorPos;
	GetCursorPos(&cursorPos);
	ScreenToClient(System::GetHWND(), &cursorPos);

	// マウスの座標を更新
	mousePos_.x = static_cast< float >(cursorPos.x);
	mousePos_.y = static_cast< float >(cursorPos.y);

	// ホイール量の更新
	mouseWheel_ = static_cast< float >(mouseState_.lZ) / 120.0f;
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
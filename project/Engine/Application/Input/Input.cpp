// Input.cpp

#include "Input.h"
#include "engine/core/System.h" // System::GetHinstance() と System::GetHWND() を使用
#include <algorithm>
#include <externals/imgui/imgui.h>
#include <Engine/core/Enviroment.h>

// シングルトンインスタンスの初期化
Input* Input::instance_ = nullptr;

//-----------------------------------------------------------------------------
/* デストラクタの実装 */
Input::~Input(){
	// キーボードデバイスの解放
	if (keyboard_){
		keyboard_->Unacquire(); // デバイスを解放する
		keyboard_.Reset();      // ComPtrのリセット
	}

	// マウスデバイスの解放
	if (mouse_){
		mouse_->Unacquire(); // デバイスを解放する
		mouse_.Reset();      // ComPtrのリセット
	}

	// ゲームパッドデバイスの解放
	if (gamepad_){
		gamepad_->Unacquire(); // デバイスを解放する
		gamepad_.Reset();      // ComPtrのリセット
	}

	// DirectInputオブジェクトの解放
	if (directInput_){
		directInput_.Reset(); // ComPtrのリセット
	}
}

//-----------------------------------------------------------------------------
/* シングルトンインスタンスの取得 */
Input* Input::GetInstance(){
	if (!instance_){
		instance_ = new Input();
	}

	return instance_;
}

//-----------------------------------------------------------------------------
/* 初期化関数 */
void Input::Initialize(){
	// インスタンスがなければ作る
	GetInstance();

	// DirectInputの初期化
	instance_->DirectInputInitialize();

	POINT pt;
	if (GetCursorPos(&pt)){
		// ウィンドウ座標に変換
		ScreenToClient(System::GetHWND(), &pt);
		instance_->mousePos_.x = static_cast< float >(pt.x);
		instance_->mousePos_.y = static_cast< float >(pt.y);
	}
}

//-----------------------------------------------------------------------------
/* 更新関数 */
void Input::Update(){
	instance_->mouseStatePre_ = instance_->mouseState_;

	// キーボード、マウス、ゲームパッドの更新
	instance_->KeyboardUpdate();
	instance_->MouseUpdate();
	instance_->GamepadUpdate();
}

//-----------------------------------------------------------------------------
/* 終了関数 */
void Input::Finalize(){
	// キーボードデバイスの解放
	if (instance_->keyboard_){
		instance_->keyboard_->Unacquire(); // デバイスを解放する
		instance_->keyboard_.Reset();      // ComPtrのリセット
	}

	// マウスデバイスの解放
	if (instance_->mouse_){
		instance_->mouse_->Unacquire(); // デバイスを解放する
		instance_->mouse_.Reset();      // ComPtrのリセット
	}

	// ゲームパッドデバイスの解放
	if (instance_->gamepad_){
		instance_->gamepad_->Unacquire(); // デバイスを解放する
		instance_->gamepad_.Reset();      // ComPtrのリセット
	}

	// DirectInputオブジェクトの解放
	if (instance_->directInput_){
		instance_->directInput_.Reset(); // ComPtrのリセット
	}

	// インスタンスの解放
	delete instance_;
	instance_ = nullptr;
}

void Input::ShowImGui(){
	ImGui::Begin("input");

	ImGui::Text("mousePos: %f, %f", instance_->mousePos_.x, instance_->mousePos_.y);
	ImGui::Text("mousePosInExecuteView:%f,%f", instance_->GetMousePosInDebugWindow().x, instance_->GetMousePosInDebugWindow().y);

	ImGui::End();
}

//-----------------------------------------------------------------------------
/* DirectInputの初期化 */
void Input::DirectInputInitialize(){
	HRESULT hr;

	// DirectInputオブジェクトの作成
	hr = DirectInput8Create(
		System::GetHinstance(),
		DIRECTINPUT_VERSION,
		IID_IDirectInput8,
		reinterpret_cast< void** >(directInput_.GetAddressOf()),
		nullptr
	);
	assert(SUCCEEDED(hr));

	/*=======================================================================================
				Keyboard
	=========================================================================================*/
	// キーボードデバイスの生成
	hr = directInput_->CreateDevice(GUID_SysKeyboard, &keyboard_, NULL);
	assert(SUCCEEDED(hr));

	// 入力データ形式のセット
	hr = keyboard_->SetDataFormat(&c_dfDIKeyboard);
	assert(SUCCEEDED(hr));

	// 排他制御レベルのセット
	hr = keyboard_->SetCooperativeLevel(
		System::GetHWND(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY
	);
	assert(SUCCEEDED(hr));

	// キーボードデバイスの取得
	hr = keyboard_->Acquire();
	assert(SUCCEEDED(hr));

	/*=======================================================================================
				Mouse
	=========================================================================================*/
	// マウスデバイスの生成
	hr = directInput_->CreateDevice(GUID_SysMouse, &mouse_, NULL);
	assert(SUCCEEDED(hr));

	// 入力データ形式のセット
	hr = mouse_->SetDataFormat(&c_dfDIMouse);
	assert(SUCCEEDED(hr));

	// 排他制御レベルのセット
	hr = mouse_->SetCooperativeLevel(
		System::GetHWND(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE
	);
	assert(SUCCEEDED(hr));

	// マウスデバイスの取得
	hr = mouse_->Acquire();
	assert(SUCCEEDED(hr));

	/*=======================================================================================
				Gamepad
	=========================================================================================*/
	// ゲームパッドデバイスの生成
	hr = directInput_->CreateDevice(GUID_Joystick, &gamepad_, nullptr);
	if (SUCCEEDED(hr)){
		// 入力データ形式のセット
		hr = gamepad_->SetDataFormat(&c_dfDIJoystick2);
		assert(SUCCEEDED(hr));

		// 排他制御レベルのセット
		hr = gamepad_->SetCooperativeLevel(
			System::GetHWND(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE
		);
		assert(SUCCEEDED(hr));

		// デバイスを初期状態で取得
		hr = gamepad_->Acquire();
		// 失敗しても続行（ゲームパッドが接続されていない場合など）
	}
}

//-----------------------------------------------------------------------------
/* キーボードの更新 */
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

//-----------------------------------------------------------------------------
/* キーが押されているかの判定 */
bool Input::PushKey(uint32_t keyNum){
	return instance_->key_[keyNum] & 0x80;
}

//-----------------------------------------------------------------------------
/* キーがトリガーされた瞬間の判定 */
bool Input::TriggerKey(uint32_t keyNum){
	return (instance_->key_[keyNum] & 0x80) && !(instance_->keyPre_[keyNum] & 0x80);
}

//-----------------------------------------------------------------------------
/* マウスの更新 */
void Input::MouseUpdate(){
	HRESULT hr = mouse_->GetDeviceState(sizeof(DIMOUSESTATE), &mouseState_);
	if (FAILED(hr)){
		if (hr == DIERR_INPUTLOST || hr == DIERR_NOTACQUIRED){
			hr = mouse_->Acquire();
		}
	}

	if (FAILED(hr)){
		mousePos_ = {0.0f, 0.0f};
	} else{

		POINT point;
		GetCursorPos(&point);
		ScreenToClient(System::GetHWND(), &point);

		// マウスの移動量を保存
		mousePos_.x = static_cast< float >(point.x);
		mousePos_.y = static_cast< float >(point.y);

		// ホイール量をfloatで更新
		mouseWheel_ = static_cast< float >(mouseState_.lZ) / 120.0f; // 1段階のホイール移動を1.0fにする
	}

}

//-----------------------------------------------------------------------------
/* マウスボタンが押されているかの判定 */
bool Input::PushMouseButton(int button){
	return (instance_->mouseState_.rgbButtons[button] & 0x80) != 0;
}

//-----------------------------------------------------------------------------
/* マウスボタンがトリガーされた瞬間の判定 */
bool Input::TriggerMouseButton(int button){
	return ((instance_->mouseState_.rgbButtons[button] & 0x80) != 0) &&
		((instance_->mouseStatePre_.rgbButtons[button] & 0x80) == 0);
}

//-----------------------------------------------------------------------------
/* 現在のマウス位置を取得 */
Vector2 Input::GetMousePosition(){
	return instance_->mousePos_;
}

Vector2 Input::GetMousePosInDebugWindow(){
	Vector2 m_ImagePos = Vector2(0, 38);       // テクスチャを描画開始するスクリーン上の座標
	Vector2 m_ImageSize = kExecuteWindowSize;   // テクスチャの幅・高さ
	Vector2 m_GameSize = kWindowSize; // 本来のゲーム描画サイズ

	// （1）ImGui全体でのマウス座標を取得
	Vector2 mousePos = GetMousePosition();

	// （2）テクスチャを貼っている領域の左上座標からの相対位置を計算
	float relativeX = mousePos.x - m_ImagePos.x;
	float relativeY = mousePos.y - m_ImagePos.y;

	// （3）テクスチャサイズに対する相対座標を、ゲーム本来の座標系に合わせてスケーリング
	float scaleX = m_GameSize.x / m_ImageSize.x;
	float scaleY = m_GameSize.y / m_ImageSize.y;

	float gamePosX = relativeX * scaleX;
	float gamePosY = relativeY * scaleY;

	return Vector2(gamePosX, gamePosY);
}

//-----------------------------------------------------------------------------
/* 現在のマウスホイールの値を取得 */
float Input::GetMouseWheel(){
	return instance_->mouseWheel_;
}

//-----------------------------------------------------------------------------
/* マウスの移動量を取得し、リセット */
Vector2 Input::GetMouseDelta(){
	// ヘッダーに追加
	static Vector2 mouseDelta_;

	// MouseUpdate()
	mouseDelta_.x = static_cast< float >(instance_->mouseState_.lX);
	mouseDelta_.y = static_cast< float >(instance_->mouseState_.lY);
	return mouseDelta_;
}

//-----------------------------------------------------------------------------
/* ゲームパッドの更新 */
void Input::GamepadUpdate(){
	if (!instance_->gamepad_) return; // 安全チェック

	// 前回の状態を保存
	instance_->gamepadStatePre_ = instance_->gamepadState_;

	// ゲームパッドの状態を取得
	XINPUT_STATE state;
	DWORD dwResult = XInputGetState(0, &state);
	if (dwResult == ERROR_SUCCESS){
		// 接続成功、状態を保存
		instance_->gamepadState_ = state.Gamepad;

		// スティック入力をキャッシュ（正規化）
		instance_->leftThumbX_ = NormalizeAxisInput(state.Gamepad.sThumbLX, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
		instance_->leftThumbY_ = NormalizeAxisInput(state.Gamepad.sThumbLY, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
		instance_->rightThumbX_ = NormalizeAxisInput(state.Gamepad.sThumbRX, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);
		instance_->rightThumbY_ = NormalizeAxisInput(state.Gamepad.sThumbRY, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);
	} else{
		// デバイス未接続の場合はゼロにリセット
		ZeroMemory(&instance_->gamepadState_, sizeof(XINPUT_GAMEPAD));
		instance_->leftThumbX_ = instance_->leftThumbY_ = instance_->rightThumbX_ = instance_->rightThumbY_ = 0.0f;
	}
}

//-----------------------------------------------------------------------------
/* スティック入力の正規化とデッドゾーン処理 */
float Input::NormalizeAxisInput(short value, short deadZone){
	// デッドゾーンの処理
	if (abs(value) < deadZone){
		return 0.0f;
	}

	// 範囲 -32768 ～ 32767 を -1.0 ～ 1.0 に変換
	float normalized = static_cast< float >(value) / 32767.0f;

	// デッドゾーンを引いた後、再スケーリング
	if (normalized > 0){
		normalized = (normalized - (static_cast< float >(deadZone) / 32767.0f)) / (1.0f - static_cast< float >(deadZone) / 32767.0f);
	} else{
		normalized = (normalized + (static_cast< float >(deadZone) / 32767.0f)) / (1.0f - static_cast< float >(deadZone) / 32767.0f);
	}

	// 範囲外の値をクリップ
	return std::clamp(normalized, -1.0f, 1.0f);
}

//-----------------------------------------------------------------------------
/* ゲームパッドボタンが押されているかの判定 */
bool Input::PushGamepadButton(PAD_BUTTON button){
	return (instance_->gamepadState_.wButtons & static_cast< WORD >(button)) != 0;
}

//-----------------------------------------------------------------------------
/* ゲームパッドボタンがトリガーされた瞬間の判定 */
bool Input::TriggerGamepadButton(PAD_BUTTON button){
	return ((instance_->gamepadState_.wButtons & static_cast< WORD >(button)) != 0) &&
		((instance_->gamepadStatePre_.wButtons & static_cast< WORD >(button)) == 0);
}

//-----------------------------------------------------------------------------
/* 左スティックの現在の状態を取得 */
Vector2 Input::GetLeftStick(){
	return {instance_->leftThumbX_, instance_->leftThumbY_};
}

//-----------------------------------------------------------------------------
/* 右スティックの現在の状態を取得 */
Vector2 Input::GetRightStick(){
	return {instance_->rightThumbX_, instance_->rightThumbY_};
}

//-----------------------------------------------------------------------------
/* 両スティックの現在の状態を取得 */
StickState Input::GetStickState(){
	StickState state;
	state.leftStick = GetLeftStick();
	state.rightStick = GetRightStick();
	return state;
}

//-----------------------------------------------------------------------------
/* 左スティックが動いているかどうかを判定する関数 */
bool Input::IsLeftStickMoved(){
	// スティックの移動量の大きさを計算
	float magnitude = std::sqrt(instance_->leftThumbX_ * instance_->leftThumbX_ + instance_->leftThumbY_ * instance_->leftThumbY_);

	// デッドゾーンを超えているかどうかを判定
	return magnitude > DEFAULT_DEAD_ZONE;
}

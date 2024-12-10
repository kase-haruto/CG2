#pragma once

#define DIRECTINPUT_VERSION 0x0800 

/* math */
#include "lib/myMath/Vector2.h"

/* c++ */
#include <wrl.h>
#include <array>
#include <dinput.h>

#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")

using Microsoft::WRL::ComPtr;

class Input{
public:
	static Input* GetInstance();

	Input(const Input&) = delete;
	Input& operator=(const Input&) = delete;

	static void Initialize();

	static void Update();

	static void Finalize();

	//=======================================================================================
	//      キーボード
public:
	/// <summary>
	/// キーボードを押した処理
	/// 長押し
	/// </summary>
	/// <param name="keyNum"></param>
	/// <returns></returns>
	static bool PushKey(uint32_t keyNum);

	/// <summary>
	/// キーボードを押した処理
	/// 短押し
	/// </summary>
	/// <param name="keyNum"></param>
	/// <returns></returns>
	static bool TriggerKey(uint32_t keyNum);

	//
	//=======================================================================================


	//=======================================================================================
	//      マウス
public:
	/// <summary>
	/// マウスボタンが押されたか
	/// </summary>
	/// <param name="button">マウスのボタン (0: 左, 1: 右, 2: 中央)</param>
	/// <returns>ボタンが押された場合は true</returns>
	static bool PushMouseButton(int button);

	/// <summary>
	/// マウスボタンの短押しを確認する
	/// </summary>
	/// <param name="button">マウスのボタン (0: 左, 1: 右, 2: 中央)</param>
	/// <returns>短押しの場合は true</returns>
	static bool TriggerMouseButton(int button);

	/// <summary>
	/// マウスの座標
	/// </summary>
	/// <returns></returns>
	static Vector2 GetMousePosition();   // マウスの座標を取得

	/// <summary>
	/// マウスのホイール移動量を取得
	/// </summary>
	static float GetMouseWheel();

	//
	//=======================================================================================


	//=======================================================================================
	//      ゲームパッド
public:
	/// <summary>
	/// ゲームパッドのボタンを押した処理
	/// 長押し
	/// </summary>
	/// <param name="button">ボタン番号</param>
	/// <returns>押されている場合は true</returns>
	static bool PushGamepadButton(int button);

	/// <summary>
	/// ゲームパッドのボタンの短押しを確認
	/// </summary>
	/// <param name="button">ボタン番号</param>
	/// <returns>短押しの場合は true</returns>
	static bool TriggerGamepadButton(int button);

	/// <summary>
	/// 左スティックの値を取得
	/// </summary>
	/// <returns>スティックの値 (-1.0f ～ 1.0f)</returns>
	static Vector2 GetLeftStick();

	/// <summary>
	/// 右スティックの値を取得
	/// </summary>
	/// <returns>スティックの値 (-1.0f ～ 1.0f)</returns>
	static Vector2 GetRightStick();

	/// <summary>
	/// トリガーの値を取得
	/// </summary>
	/// <returns>トリガーの値 (0.0f ～ 1.0f)</returns>
	static float GetTrigger();

	//
	//=======================================================================================


private:
	Input() = default;
	~Input();

	/// <summary>
	/// directInputの初期化
	/// </summary>
	void DirectInputInitialize();

	/// <summary>
	/// キーボードの更新
	/// </summary>
	void KeyboardUpdate();

	void MouseUpdate();

private:
	/// <summary>
	/// ゲームパッドの更新
	/// </summary>
	void GamepadUpdate();

private:
	//インスタンス
	static Input* instance_;

	ComPtr<IDirectInput8> directInput_ = nullptr;


private:
	/*------------------------
		キーボード
	------------------------*/
	ComPtr<IDirectInputDevice8> keyboard_ = nullptr;
	std::array<BYTE, 256> key_;
	std::array<BYTE, 256> keyPre_;

private:
	/*------------------------
	  マウス
	------------------------*/
	ComPtr<IDirectInputDevice8> mouse_ = nullptr;
	DIMOUSESTATE mouseState_;
	DIMOUSESTATE mouseStatePre_;
	Vector2 mousePos_;
	float mouseWheel_ = 0.0f;

	/*------------------------
	 パッド
	------------------------*/
private:
	ComPtr<IDirectInputDevice8> gamepad_ = nullptr;
	DIJOYSTATE2 gamepadState_;
	DIJOYSTATE2 gamepadStatePre_;
};
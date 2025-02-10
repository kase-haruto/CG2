// Input.h

#pragma once

#define DIRECTINPUT_VERSION 0x0800 

#include "lib/myMath/Vector2.h" // 必要に応じて実装をリンク
#include <wrl.h>
#include <array>
#include <dinput.h>
#include <XInput.h>
#include <unordered_map>
#include <string>
#include <cmath> // 追加: sqrt関数を使用するため

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "xinput.lib")

// デッドゾーンのデフォルト値
constexpr float DEFAULT_DEAD_ZONE = 0.2f;

enum class PAD_BUTTON : WORD{
    A = XINPUT_GAMEPAD_A,
    B = XINPUT_GAMEPAD_B,
    X = XINPUT_GAMEPAD_X,
    Y = XINPUT_GAMEPAD_Y,
    LB = XINPUT_GAMEPAD_LEFT_SHOULDER,
    RB = XINPUT_GAMEPAD_RIGHT_SHOULDER,
    BACK = XINPUT_GAMEPAD_BACK,
    START = XINPUT_GAMEPAD_START,
    L_STICK = XINPUT_GAMEPAD_LEFT_THUMB,
    R_STICK = XINPUT_GAMEPAD_RIGHT_THUMB,
    DPAD_UP = XINPUT_GAMEPAD_DPAD_UP,
    DPAD_DOWN = XINPUT_GAMEPAD_DPAD_DOWN,
    DPAD_LEFT = XINPUT_GAMEPAD_DPAD_LEFT,
    DPAD_RIGHT = XINPUT_GAMEPAD_DPAD_RIGHT,
    LT = 0x0001, // Example value, adjust as needed
    RT = 0x0002, // Example value, adjust as needed
    COUNT    // ボタン数を取得可能にするための要素
};

using Microsoft::WRL::ComPtr;

// スティックの状態を格納する構造体
struct StickState{
    Vector2 leftStick;  // 左スティックの状態
    Vector2 rightStick; // 右スティックの状態
};

class Input{
public:
    // インスタンスの取得
    static Input* GetInstance();

    // コピー禁止
    Input(const Input&) = delete;
    Input& operator=(const Input&) = delete;

    // 初期化、更新、終了
    static void Initialize();
    static void Update();
    static void Finalize();

    // キーボード
    static bool PushKey(uint32_t keyNum);
    static bool TriggerKey(uint32_t keyNum);

    // マウス
    static bool PushMouseButton(int button);
    static bool TriggerMouseButton(int button);
    static Vector2 GetMousePosition();
    static float GetMouseWheel();
    static Vector2 GetMouseDelta();

    // ゲームパッド
    static bool PushGamepadButton(PAD_BUTTON button);
    static bool TriggerGamepadButton(PAD_BUTTON button);
    static Vector2 GetLeftStick();
    static Vector2 GetRightStick();
    static StickState GetStickState(); // 両スティックの状態を取得

    // 左スティックが動いているかどうかを判定する関数
    static bool IsLeftStickMoved();

private:
    Input() = default;
    ~Input();

    // DirectInput初期化
    void DirectInputInitialize();

    // 各デバイスの更新
    void KeyboardUpdate();
    void MouseUpdate();
    void GamepadUpdate();

    float NormalizeAxisInput(short value, short deadZone);

private:
    static Input* instance_; // シングルトンインスタンス

    // DirectInputオブジェクト
    ComPtr<IDirectInput8> directInput_ = nullptr;

    // キーボードデバイス
    ComPtr<IDirectInputDevice8> keyboard_ = nullptr;
    std::array<BYTE, 256> key_ {};    // 現在のキー状態
    std::array<BYTE, 256> keyPre_ {}; // 前回のキー状態

    // マウスデバイス
    ComPtr<IDirectInputDevice8> mouse_ = nullptr;
    DIMOUSESTATE mouseState_ {};
    DIMOUSESTATE mouseStatePre_ {};
    Vector2 mousePos_ {};
    float mouseWheel_ = 0.0f;

    // ゲームパッドデバイス
    ComPtr<IDirectInputDevice8> gamepad_ = nullptr;
    XINPUT_GAMEPAD gamepadState_ {};
    XINPUT_GAMEPAD gamepadStatePre_ {};
    float leftThumbX_ = 0.0f;  // 初期化を追加
    float leftThumbY_ = 0.0f;  // 初期化を追加
    float rightThumbX_ = 0.0f; // 初期化を追加
    float rightThumbY_ = 0.0f; // 初期化を追加
};

#pragma once

#define DIRECTINPUT_VERSION 0x0800 
#include <dinput.h>

#include <wrl.h>
#include <array>

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
    //      ゲームパッド
public:

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

private:
    //インスタンス
    static Input* instance_;

    ComPtr<IDirectInput8> directInput_ = nullptr;

    /*------------------------
        キーボード
    ------------------------*/
    ComPtr<IDirectInputDevice8> keyboard_ = nullptr;
    std::array<BYTE, 256> key_;
    std::array<BYTE, 256> keyPre_;

};

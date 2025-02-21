#pragma once
#include <Windows.h>
#include <cstdint>
#include <string>

class WinApp{
public:
    // ウィンドウプロシージャ
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

    // コンストラクタ
    WinApp(const int wWidth, const int wHeight, const std::string windowName);
    // デストラクタ
    ~WinApp();
    // ウィンドウを作成
    void CreateWnd();
    // ウィンドウハンドルを取得
    HWND GetHWND() const{ return hwnd; }
    // ゲームウィンドウの破棄
    void TerminateGameWindow();
    // メッセージ処理
    bool ProcessMessage();
    // フルスクリーン切り替え
    void SetFullScreen(bool enable);

private:
    // ウィンドウの情報
    WNDCLASSEX wc {};
    // ウィンドウのサイズ
    RECT wrc = {0, 0, 1280, 720};
    // ウィンドウのハンドル
    HWND hwnd {};
    // ウィンドウタイトル
    std::string windowName_;
    // フルスクリーン状態
    bool isFullScreen = false;
    // ウィンドウの元の位置とサイズ
    WINDOWPLACEMENT windowPlacement = {sizeof(WINDOWPLACEMENT)};
};

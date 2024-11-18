#include "WinApp.h"

/* lib */
#include "lib/myFunc/ConvertString.h"

/* externals */
#include "externals/imgui/imgui.h"
#include "externals/imgui/imgui_impl_dx12.h"
#include "externals/imgui/imgui_impl_win32.h"

/* c++ */
#include <tchar.h>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hand, UINT msg, WPARAM wparam, LPARAM lparam);

//コンストラクタ
WinApp::WinApp(const int wWidth, const int wHeight, const std::string windowName) {
    wrc.right = wWidth;
    wrc.bottom = wHeight;
    windowName_ = windowName;

	CreateWnd();
}

//デストラクタ
WinApp::~WinApp() {
    CloseWindow(hwnd);
}

//ウィンドウプロシージャ
LRESULT CALLBACK WindowProc(HWND hand, UINT msg, WPARAM wparam, LPARAM lparam) {
    if (ImGui_ImplWin32_WndProcHandler(hand, msg, wparam, lparam)){
        return true;
    }
    
    //メッセージに応じてゲームの画面の制御を行う
	switch (msg) {
	case WM_DESTROY:
		//osに対して、アプリの終了を伝える
		PostQuitMessage(0);
		return 0;
	}

	//標準のメッセージ処理を行う
	return DefWindowProc(hand, msg, wparam, lparam);
}

bool WinApp::ProcessMessage() {
    MSG msg{}; // メッセージ

    if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) // メッセージがある？
    {
        TranslateMessage(&msg); // キー入力メッセージの処理
        DispatchMessage(&msg);  // ウィンドウプロシージャにメッセージを送る
    }

    if (msg.message == WM_QUIT) // 終了メッセージが来たらループを抜ける
    {
        return true;
    }

    return false;
}

void WinApp::CreateWnd(void) {
    // ウィンドウクラスの初期化
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(nullptr);
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.lpszClassName = _T("WinApp");

    // ウィンドウクラスの登録
    RegisterClassEx(&wc);

    // サイズの補正
    AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

    // ウィンドウの作成
    hwnd = CreateWindow(
        wc.lpszClassName,
        ConvertString(windowName_).c_str(),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        wrc.right - wrc.left,
        wrc.bottom - wrc.top,
        nullptr,
        nullptr,
        wc.hInstance,
        nullptr
    );
    // ウィンドウの表示
    ShowWindow(hwnd, SW_SHOW);
}

/// <summary>
/// ゲームウィンドウの破棄
/// </summary>
void WinApp::TerminateGameWindow(){
    // ウィンドウクラスを登録解除
    UnregisterClass(wc.lpszClassName, wc.hInstance);

    // COM 終了
    CoUninitialize();
}
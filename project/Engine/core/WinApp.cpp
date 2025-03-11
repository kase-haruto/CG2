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

// コンストラクタ
WinApp::WinApp(const int wWidth, const int wHeight, const std::string windowName){
    wrc.right = wWidth;
    wrc.bottom = wHeight;
    windowName_ = windowName;
    CreateWnd();
}

// デストラクタ
WinApp::~WinApp(){
    CloseWindow(hwnd);
}

// ウィンドウプロシージャ
LRESULT CALLBACK WinApp::WindowProc(HWND hand, UINT msg, WPARAM wparam, LPARAM lparam){
    if (ImGui_ImplWin32_WndProcHandler(hand, msg, wparam, lparam)){
        return true;
    }

    switch (msg){
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;

        case WM_KEYDOWN:
            if (wparam == VK_RETURN && (GetAsyncKeyState(VK_MENU) & 0x8000)){ // Alt + Enter でフルスクリーン切り替え
                WinApp* app = reinterpret_cast< WinApp* >(GetWindowLongPtr(hand, GWLP_USERDATA));
                if (app){
                    app->SetFullScreen(!app->isFullScreen);
                }
            }
            break;
    }

    return DefWindowProc(hand, msg, wparam, lparam);
}

bool WinApp::ProcessMessage(){
    MSG msg {}; // メッセージ

    while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)){
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return msg.message == WM_QUIT;
}

void WinApp::CreateWnd(){
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(nullptr);
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.lpszClassName = _T("WinApp");

    RegisterClassEx(&wc);

    AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

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

    SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast< LONG_PTR >(this));

    ShowWindow(hwnd, SW_SHOW);
}

void WinApp::TerminateGameWindow(){
    UnregisterClass(wc.lpszClassName, wc.hInstance);
    CoUninitialize();
}

void WinApp::SetFullScreen(bool enable){
    if (isFullScreen == enable) return;

    isFullScreen = enable;

    if (enable){
        GetWindowPlacement(hwnd, &windowPlacement);
        SetWindowLong(hwnd, GWL_STYLE, WS_POPUP | WS_VISIBLE);

        int screenWidth = GetSystemMetrics(SM_CXSCREEN);
        int screenHeight = GetSystemMetrics(SM_CYSCREEN);
        SetWindowPos(hwnd, HWND_TOP, 0, 0, screenWidth, screenHeight, SWP_FRAMECHANGED);

        // **ここで ImGui の解像度を更新**
        ImGui::GetIO().DisplaySize = ImVec2(( float ) screenWidth, ( float ) screenHeight);
    } else{
        SetWindowLong(hwnd, GWL_STYLE, WS_OVERLAPPEDWINDOW | WS_VISIBLE);
        SetWindowPlacement(hwnd, &windowPlacement);
        SetWindowPos(hwnd, nullptr, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED);

        // **ウィンドウモードのときも更新**
        RECT rect;
        GetClientRect(hwnd, &rect);
        ImGui::GetIO().DisplaySize = ImVec2(( float ) (rect.right - rect.left), ( float ) (rect.bottom - rect.top));
    }
}


#pragma once
#include<Windows.h>
#include<cstdint>
#include<string>

class WinApp {
public://静的メンバ変数
	// ウィンドウサイズ
	static const int kWindowWidth = 1280; // 横幅
	static const int kWindowHeight = 720; // 縦幅

	//ウィンドウプロシージャ
	static LRESULT WinAppProcedure(HWND hwnd, UINT msg, WPARAM wparm, LPARAM lparam);


public://メンバ関数
	//コンストラクタ
	WinApp(const int wWidth,const int wHeight,const std::string windowName);
	//デストラクタ
	~WinApp();
	//この関数内でウィンドウを作成する
	void CreateWnd(void);
	//ウィンドウハンドルの取得
	HWND GetHWND()const { return hwnd; }
	/// <summary>
	/// ゲームウィンドウの破棄
	/// </summary>
	void TerminateGameWindow();

	bool ProcessMessage();
private:
	//ウィンドウの情報を格納する関数
	WNDCLASSEX wc;
	//ウィンドウのサイズを格納する構造体
	RECT wrc = { 0,0,1280,720 };
	//ウィンドウのハンドル
	HWND hwnd;
	//ウィンドウタイトル
	std::string windowName_ {};
};


#pragma once
#include<Windows.h>
#include<cstdint>

class WinApp {
public://静的メンバ変数
	// ウィンドウサイズ
	static const int kWindowWidth = 1280; // 横幅
	static const int kWindowHeight = 720; // 縦幅

	//ウィンドウプロシージャ
	static LRESULT WinAppProcedure(HWND hwnd, UINT msg, WPARAM wparm, LPARAM lparam);
	
public://静的メンバ関数
	/// <summary>
	/// シングルトンインスタンスの取得
	/// </summary>
	/// <returns>シングルトンインスタンス</returns>
	static WinApp* GetInstance();

	
public://メンバ関数
	//コンストラクタ
	WinApp();
	//デストラクタ
	~WinApp();
	//この関数内でウィンドウを作成する
	void CreateWnd(void);
	//ウィンドウハンドルの取得
	HWND GetHWND()const { return hwnd; }

	bool ProcessMessage();
private:
	//ウィンドウの情報を格納する関数
	WNDCLASSEX wc;
	//ウィンドウのサイズを格納する構造体
	RECT wrc = { 0,0,1280,720 };
	//ウィンドウのハンドル
	HWND hwnd;

private:

	WinApp(const WinApp&) = delete;
	const WinApp& operator=(const WinApp&) = delete;
};


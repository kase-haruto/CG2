#pragma once
#include<Windows.h>
#include<cstdint>

class WinApp {
private:
	//ウィンドウプロシージャ
	static LRESULT WinAppProcedure(HWND hwnd, UINT msg, WPARAM wparm, LPARAM lparam);
	//この関数内でウィンドウを作成する
	void CreateWnd(void);

	//ウィンドウの情報を格納する関数
	WNDCLASSEX wc;
	//ウィンドウのサイズを格納する構造体
	RECT wrc = { 0,0,1280,720 };
	//ウィンドウのハンドル
	HWND hwnd;

public:
	//コンストラクタ
	WinApp();
	//デストラクタ
	~WinApp();

	//ウィンドウハンドルの取得
	HWND Get(void)const {
		return hwnd;
	}
};


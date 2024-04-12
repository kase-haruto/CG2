#include<Windows.h>
#include"WinApp.h"


int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	
	WinApp* win = new WinApp();
	MSG msg{};


	while (msg.message != WM_QUIT) {
		if (PeekMessage(&msg,nullptr,0,0,PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		/*==========================================================
				ゲームの処理
		===========================================================*/
		else {



		}
	}


	delete win;
	return 0;
}
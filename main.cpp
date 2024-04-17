#include<Windows.h>
#include"WinApp.h"
#include"DirectX12.h"

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	WinApp* win = win->GetInstance();
	MSG msg{};
	DirectX12* directX = directX->GetInstance();

	directX->Initialize(win);
	
	while (msg.message != WM_QUIT) {
		if (PeekMessage(&msg,nullptr,0,0,PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		

		/*==========================================================
				ゲームの処理
		===========================================================*/
		else {
			directX->PreDraw();

			directX->PostDraw();

		}
	}


	return 0;
}
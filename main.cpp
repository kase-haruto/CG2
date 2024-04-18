#include<Windows.h>
#include"WinApp.h"
#include"DirectXCommon.h"

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	WinApp* win = win->GetInstance();
	MSG msg{};
	DirectXCommon* directX = directX->GetInstance();

	directX->Initialize(win,1280,720);

	while (win->ProcessMessage()) {

		//フレームの開始
		directX->PreDraw();


		
		//フレームの終了
		directX->PostDraw();
	}

	directX->Finalize();
	return 0;
}
#include<Windows.h>
#include"WinApp.h"
#include"DirectX12.h"

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	WinApp* win = win->GetInstance();
	MSG msg{};
	DirectX12* directX = directX->GetInstance();

	directX->Initialize(win);

	while (win->ProcessMessage()) {

		//フレームの開始
		directX->PreDraw();



		//フレームの終了
		directX->PostDraw();


	}

	directX->Finalize();
	return 0;
}
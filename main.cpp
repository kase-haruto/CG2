#include<Windows.h>
#include"WinApp.h"
#include"DirectXCommon.h"

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	WinApp* win = win->GetInstance();
	MSG msg{};
	DirectXCommon* directX = directX->GetInstance();

	directX->Initialize(win,1280,720);

	while (win->ProcessMessage()==0) {

		//フレームの開始
		directX->PreDraw();

		//三角形の更新
		directX->UpdatePolygon();

		//三角形の描画
		directX->DrawPolygon();
		
		//フレームの終了
		directX->PostDraw();
	}

	directX->Finalize();
	return 0;
}
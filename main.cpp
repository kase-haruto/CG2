#include<Windows.h>
#include"WinApp.h"
#include"DirectXCommon.h"
#include"TextureManager.h"
#include"ViewProjection.h"
#include"ImGuiManager.h"
#include"imgui.h"
#include"TestScene.h"

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int){
	//comの初期化
	CoInitializeEx(0, COINIT_MULTITHREADED);

	WinApp* win = win->GetInstance();
	MSG msg {};
	DirectXCommon* dxCommon = dxCommon->GetInstance();
	dxCommon->Initialize(win, 1280, 720);
	dxCommon->Pipeline();

#pragma region 汎用機能初期化

#ifdef _DEBUG
	// ImGuiの初期化
	ImGuiManager* imguiManager = ImGuiManager::GetInstance();
	imguiManager->Initialize(win, dxCommon);
#endif // _DEBUG

#pragma endregion

	//textureManagerの初期化
	TextureManager::GetInstance()->Initialize(dxCommon->GetDevice(), imguiManager);
	TextureManager::GetInstance()->LoadTexture("./Resources/uvChecker.png");
	TextureManager::GetInstance()->LoadTexture("./Resources/monsterBall.png");
	//テクスチャの転送
	TextureManager::GetInstance()->TransferTexture();

	//シーンの初期化
	std::unique_ptr<TestScene> scene = std::make_unique<TestScene>();
	scene->Initialize();

	while (win->ProcessMessage() == 0){
		//フレームの開始
		dxCommon->PreDraw();
		// ImGui受付開始
		imguiManager->Begin();


		scene->Update();

		scene->Draw();


		//=================================
		//フレーム終了時の処理
		//imguiのコマンドを積む
		imguiManager->End();
		//ImGui描画
		imguiManager->Draw();
		//フレームの終了
		dxCommon->PostDraw();		
	}

	// ImGui解放
	imguiManager->Finalize();
	dxCommon->Finalize();
	delete win;
	CoUninitialize();
	return 0;
}
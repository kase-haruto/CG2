#include<Windows.h>
#include"WinApp.h"
#include"DirectXCommon.h"
#include"TextureManager.h"
#include"FogEffect.h"
#include"ViewProjection.h"
#include"ImGuiManager.h"
#include"imgui.h"
#include"Sprite.h"


int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int){
	//comの初期化
	CoInitializeEx(0, COINIT_MULTITHREADED);

	WinApp* win = win->GetInstance();
	MSG msg {};
	DirectXCommon* dxCommon = dxCommon->GetInstance();
	dxCommon->Initialize(win, 1280, 720);
	dxCommon->Pipeline();

	FogEffect* fog = new FogEffect(dxCommon);
	Sprite* sprite = new Sprite(dxCommon);
	sprite->Initialize();

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

	while (win->ProcessMessage() == 0){

		// ImGui受付開始
		imguiManager->Begin();
		//三角形の更新
		dxCommon->UpdatePolygon();
		//スプライト
		sprite->Update();
		// ImGui受付終了
		imguiManager->End();




		//フレームの開始
		dxCommon->PreDraw();
		//ImGui描画
		imguiManager->Draw();
		//三角形の描画
		dxCommon->DrawSphere();
		//sprite描画
		sprite->Draw();
		//フレームの終了
		dxCommon->PostDraw();
	}

	// ImGui解放
	imguiManager->Finalize();
	dxCommon->Finalize();
	delete win, fog,sprite;
	CoUninitialize();
	return 0;
}
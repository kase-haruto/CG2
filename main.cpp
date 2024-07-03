#include<Windows.h>
#include"WinApp.h"
#include"DirectXCommon.h"
#include"TextureManager.h"
#include"FogEffect.h"
#include"ViewProjection.h"
#include"ImGuiManager.h"
#include"imgui.h"
#include"Sprite.h"
#include"DirectionalLight.h"
#include"Model.h"

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int){
	//comの初期化
	CoInitializeEx(0, COINIT_MULTITHREADED);

	WinApp* win = win->GetInstance();
	MSG msg {};
	DirectXCommon* dxCommon = dxCommon->GetInstance();
	dxCommon->Initialize(win, 1280, 720);
	dxCommon->Pipeline();

	Sprite* sprite = new Sprite(dxCommon);
	sprite->Initialize();

	std::unique_ptr<DirectionalLight> light= std::make_unique<DirectionalLight>();
	light->Initialize(dxCommon);

	std::unique_ptr<Model>model = std::make_unique<Model>();
	model->Initialize(dxCommon);

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
		//フレームの開始
		dxCommon->PreDraw();
		// ImGui受付開始
		imguiManager->Begin();
		light->Update();
		//モデルの更新
		model->Update();
		//スプライト
		//sprite->Update();




		//ライトの処理
		light->Render();
		//モデルの描画
		model->Draw();
		//sprite描画
		//sprite->Draw();

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
	delete win,sprite;
	CoUninitialize();
	return 0;
}
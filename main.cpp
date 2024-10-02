#include<Windows.h>
#include"WinApp.h"
#include"System.h"
#include"GameScene.h"

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int){
	//comの初期化
	CoInitializeEx(0, COINIT_MULTITHREADED);

	//メインシステムの初期化
	std::unique_ptr<System> system = std::make_unique<System>();
	system->Initialize(1280, 720);

	//シーンの初期化
	std::unique_ptr<GameScene> scene = std::make_unique<GameScene>();
	scene->Initialize();
	

	while (!system->ProcessMessage()){

		//===========================
		//フレーム前処理
		system->BeginFrame();


		//==========================
		//更新処理
		scene->Update();

		//==========================
		//描画処理
		scene->Draw();
		

		//==========================
		//フレーム後処理
		system->EndFrame();
	}

	scene->Finalize();
	system->Finalize();
	return 0;
}
#include "engine/core/EngineController.h"
#include "engine/core/Enviroment.h"
#include "../scene/SceneManager.h"
#include <Engine/core/Input.h>

//===================================================================*/
// Engine全体の初期化処理
//===================================================================*/
void EngineController::Initialize(HINSTANCE hInstance){
	// comの初期化
	CoInitializeEx(0, COINIT_MULTITHREADED);

	// engineの初期化
	system_ = std::make_unique<System>();
	system_->Initialize(hInstance, kWindowWidth, kWindowHeight, windowTitle);

	// UI初期化
	auto ui = EngineUI::GetInstance();

	// シーンマネージャ初期化
	sceneManager_ = std::make_unique<SceneManager>(system_->GetDxCore());
	sceneManager_->SetEngineUI(ui);
	sceneManager_->Initialize();
}

//===================================================================*/
// メインループ処理
//===================================================================*/
void EngineController::Run(){
	// メインループ
	while (!system_->ProcessMessage()){
		// 描画前処理
		system_->BeginFrame();

		// UI描画
		EngineUI::Render();

		// シーンの更新
		sceneManager_->Update();

		// シーンの描画
		sceneManager_->Draw();

		// 描画後処理
		system_->EndFrame();

		if (Input::TriggerKey(DIK_ESCAPE)){
			break;
		}
	}
}

//===================================================================*/
// 終了処理
//===================================================================*/
void EngineController::Finalize(){
	//終了処理
	system_->Finalize();
	sceneManager_.reset();
	EngineUI::Finalize();
	CoUninitialize();
}

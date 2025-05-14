#include "EngineController.h"
/* ========================================================================
/*	include space
/* ===================================================================== */
// engine
#include <Engine/Application/System/Enviroment.h>
#include <Engine/Scene/System/SceneManager.h>
#include <Engine/Application/Input/Input.h>
#include <Engine/Application/UI/EngineUI/Core/EngineUICore.h>

//===================================================================*/
// Engine全体の初期化処理
//===================================================================*/
void EngineController::Initialize(HINSTANCE hInstance){
	// comの初期化
	CoInitializeEx(0, COINIT_MULTITHREADED);


	// engineの初期化
	system_ = std::make_unique<System>();
	system_->Initialize(hInstance, kWindowWidth, kWindowHeight, windowTitle);

	// engineUIの初期化
	engineUICore_ = std::make_unique<EngineUICore>();
	engineUICore_->Initialize();

	system_->SetEngineUICore(engineUICore_.get());
	system_->InitializeEditor();


	// シーンマネージャ初期化
	sceneManager_ = std::make_unique<SceneManager>(system_->GetDxCore());
	sceneManager_->SetEngineUI(engineUICore_.get());
	sceneManager_->Initialize();
}

bool EngineController::Update() {
	return false;
}

void EngineController::Render() {}

//===================================================================*/
// メインループ処理
//===================================================================*/
void EngineController::Run(){
	// メインループ
	while (!system_->ProcessMessage()){
		// 描画前処理
		system_->BeginFrame();

		// UI描画
		engineUICore_->Render();

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
	CoUninitialize();
}

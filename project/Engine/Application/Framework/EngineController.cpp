#include "EngineController.h"
/* ========================================================================
/*	include space
/* ===================================================================== */
// engine
#include <Engine/Application/System/Enviroment.h>
#include <Engine/Scene/System/SceneManager.h>
#include <Engine/Application/Input/Input.h>
#include <Engine/Application/UI/EngineUI/Core/EngineUICore.h>
#include <Engine/Editor/PostProcessEditor.h>

//===================================================================*/
// Engine全体の初期化処理
//===================================================================*/
void EngineController::Initialize(HINSTANCE hInstance){
	// comの初期化
	CoInitializeEx(0, COINIT_MULTITHREADED);


	// engineの初期化
	system_ = std::make_unique<System>();
	system_->Initialize(hInstance, kWindowWidth, kWindowHeight, windowTitle);

	// graphicsシステムの初期化
	graphicsSystem_ = std::make_unique<GraphicsSystem>();
	graphicsSystem_->Initialize();

	system_->InitializePostProcess(graphicsSystem_->GetPipelineService());

	// engineUIの初期化
	engineUICore_ = std::make_unique<EngineUICore>();
	engineUICore_->Initialize();
	system_->SetEngineUICore(engineUICore_.get());

	// engineEditorの初期化
	editorCollection_ = std::make_unique<EditorCollection>();
	editorCollection_->SetEngineUICore(engineUICore_.get());
	editorCollection_->InitializeEditors();
	auto ppCollection = dynamic_cast< PostProcessEditor* >(editorCollection_->GetEditor(EditorCollection::EditorType::PostProcess));
	ppCollection->SetPostEffectCollection(system_->GetPostProcessCollection());

	// シーンマネージャ初期化
	sceneManager_ = std::make_unique<SceneManager>(system_->GetDxCore(),graphicsSystem_.get());
	sceneManager_->SetEngineUI(engineUICore_.get());
	sceneManager_->Initialize();
}

bool EngineController::Update() {
	return false;
}

void EngineController::BeginUpdate(){
	// 描画前処理
	system_->BeginFrame();

	// UI描画
	engineUICore_->Render();
}

void EngineController::EndUpdate(){
	editorCollection_->UpdateEditors();
}

void EngineController::Render() {}

//===================================================================*/
// メインループ処理
//===================================================================*/
void EngineController::Run(){
	// メインループ
	while (!system_->ProcessMessage()){
		
		BeginUpdate();

		// シーンの更新
		sceneManager_->Update();

		EndUpdate();

		// シーンの描画
		sceneManager_->Draw();

		// 描画後処理
		system_->EndFrame(graphicsSystem_->GetPipelineService()->GetPipelineSet(PipelineTag::PostProcess::CopyImage));

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
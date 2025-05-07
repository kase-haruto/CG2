#include "SceneManager.h"
#include "TestScene.h"
#include "GameScene.h"
#include "SceneFactory.h"

#include "engine/objects/SceneObjectManager.h"
#include <Engine/graphics/camera/BaseCamera.h>
#include <Engine/core/DirectX/RenderTarget/IRenderTarget.h>
#include <Engine/physics/light/LightManager.h>
#include <Engine/graphics/camera/CameraManager.h>

SceneManager::SceneManager(DxCore* dxCore)
	: pDxCore_(dxCore){
	// ここでシーンをすべて生成しておく場合
	for (int i = 0; i < static_cast< int >(SceneType::count); ++i){
		scenes_[i] = SceneFactory::CreateScene(static_cast< SceneType >(i), pDxCore_);
	}

	// 最初は TITLE シーンにしておく
	currentSceneNo_ = static_cast< int >(SceneType::PLAY);
	nextSceneNo_ = currentSceneNo_;
}

SceneManager::~SceneManager(){}

void SceneManager::Initialize(){
	// シーン切り替えパネルを作成 (UI がある場合)
	if (pEngineUI_){
		sceneSwitchPanel_ = std::make_unique<SceneSwitcherPanel>(this);

		sceneSwitchPanel_->AddSceneOption("Game Scene", SceneType::PLAY);
		sceneSwitchPanel_->AddSceneOption("Test Scene", SceneType::TEST);
		// ... ほかのシーンも必要なら追加 ...

		// UIにパネルを追加
		pEngineUI_->AddPanel(std::move(sceneSwitchPanel_));
	}

	// まずは最初のシーンを初期化
	scenes_[currentSceneNo_]->SetSceneManager(this);
	scenes_[currentSceneNo_]->SetEngineUI(pEngineUI_);
	scenes_[currentSceneNo_]->Initialize();
}

void SceneManager::Update(){
	// シーン切り替えチェック
	if (currentSceneNo_ != nextSceneNo_){
		// いったん現在シーンをクリーンアップ
		scenes_[currentSceneNo_]->CleanUp();

		// シーン番号を更新
		currentSceneNo_ = nextSceneNo_;

		// 新しいシーンに UI と SceneManager をセットして Initialize
		scenes_[currentSceneNo_]->SetSceneManager(this);
		scenes_[currentSceneNo_]->SetEngineUI(pEngineUI_);
		scenes_[currentSceneNo_]->Initialize();
	}

	// 現在のシーンを更新
	scenes_[currentSceneNo_]->Update();
}

void SceneManager::Draw(){
	// MainCamera は既存の Draw() に任せる（CameraManager::SetType(Type_Default) が内部で使われる）
	CameraManager::GetInstance()->SetType(Type_Default);
	BaseCamera* mainCam = CameraManager::GetInstance()->GetCamera3d();
	// 現在のシーンを描画
	scenes_[currentSceneNo_]->Draw(mainCam);

	// DebugCamera を明示的に描画
//auto* debugCam = CameraManager::GetInstance()->GetDebugCamera();
//auto* debugRT = pDxCore_->GetRenderTargetCollection().Get("DebugView");
//DrawToCamera(debugCam, debugRT);
}

void SceneManager::DrawToCamera(BaseCamera* camera, IRenderTarget* target){
	auto* cmd = pDxCore_->GetCommandList().Get();

	// 出力先RT設定
	target->SetRenderTarget(cmd);

	// 指定カメラでパイプライン設定
	camera->SetCommand(cmd, PipelineType::Object3D);

	// ライト設定（共通）
	LightManager::GetInstance()->SetCommand(cmd, LightType::Directional, PipelineType::Object3D);
	LightManager::GetInstance()->SetCommand(cmd, LightType::Point, PipelineType::Object3D);

	Matrix4x4 viewProjection = camera->GetViewProjection();

	if (auto* baseScene = dynamic_cast< BaseScene* >(scenes_[currentSceneNo_].get())){
		baseScene->GetSceneContext()->meshRenderer_->DrawAll(viewProjection);
	}

}


void SceneManager::RequestSceneChange(SceneType nextScene){
	nextSceneNo_ = static_cast< int >(nextScene);
	SceneObjectManager::GetInstance()->ClearGameObjects();

}

void SceneManager::SetCurrentScene(std::unique_ptr<IScene> newScene){
	// いったん現在シーンをクリーンアップ
	scenes_[currentSceneNo_]->CleanUp();

	// シーン番号を更新
	currentSceneNo_ = nextSceneNo_;

	// 新しいシーンに UI と SceneManager をセットして Initialize
	scenes_[currentSceneNo_]->SetSceneManager(this);
	scenes_[currentSceneNo_]->SetEngineUI(pEngineUI_);
	scenes_[currentSceneNo_]->Initialize();
}

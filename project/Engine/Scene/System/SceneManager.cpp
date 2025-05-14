#include "SceneManager.h"

// scene
#include <Engine/Scene/Game/GameScene.h>
#include <Engine/Scene/System/SceneFactory.h>
#include <Engine/Scene/Test/TestScene.h>

// engine
#include <Engine/Graphics/Camera/Manager/CameraManager.h>
#include <Engine/Graphics/RenderTarget/Interface/IRenderTarget.h>
#include <Engine/objects/SceneObjectManager.h>
#include <Engine/physics/light/LightManager.h>

SceneManager::SceneManager(DxCore* dxCore)
	: pDxCore_(dxCore) {
	// ここでシーンをすべて生成しておく場合
	for (int i = 0; i < static_cast<int>(SceneType::count); ++i) {
		scenes_[i] = SceneFactory::CreateScene(static_cast<SceneType>(i), pDxCore_);
	}

	// 最初は TITLE シーンにしておく
	currentSceneNo_ = static_cast<int>(SceneType::TEST);
	nextSceneNo_ = currentSceneNo_;
}

SceneManager::~SceneManager() {}

void SceneManager::Initialize() {
	// シーン切り替えパネルを作成 (UI がある場合)
	if (pEngineUI_) {
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

void SceneManager::Update() {
	// シーン切り替えチェック
	if (currentSceneNo_ != nextSceneNo_) {
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

void SceneManager::Draw() {
	CameraManager::GetInstance()->SetType(Type_Default);
	auto* gameRT = pDxCore_->GetRenderTargetCollection().Get("Offscreen");
	// 現在のシーンを描画
	DrawForRenderTarget(gameRT);

#ifdef _DEBUG
	// DebugCamera を明示的に描画
	CameraManager::GetInstance()->SetType(Type_Debug);
	auto* debugRT = pDxCore_->GetRenderTargetCollection().Get("DebugView");
	DrawForRenderTarget(debugRT);
#endif // _DEBUG
}

void SceneManager::DrawForRenderTarget(IRenderTarget* target) {
	auto* cmd = pDxCore_->GetCommandList().Get();

	// 出力先RT設定
	target->SetRenderTarget(cmd);
	target->Clear(cmd);

	scenes_[currentSceneNo_]->Draw();
}

void SceneManager::RequestSceneChange(SceneType nextScene) {
	nextSceneNo_ = static_cast<int>(nextScene);
	SceneObjectManager::GetInstance()->ClearGameObjects();

}

void SceneManager::SetCurrentScene(std::unique_ptr<IScene> newScene) {
	// いったん現在シーンをクリーンアップ
	scenes_[currentSceneNo_]->CleanUp();

	// シーン番号を更新
	currentSceneNo_ = nextSceneNo_;

	// 新しいシーンに UI と SceneManager をセットして Initialize
	scenes_[currentSceneNo_]->SetSceneManager(this);
	scenes_[currentSceneNo_]->SetEngineUI(pEngineUI_);
	scenes_[currentSceneNo_]->Initialize();
}

#include "SceneManager.h"
#include "TestScene.h"

//===================================================================*/
//                   デストラクタ
//===================================================================*/
SceneManager::~SceneManager(){
	if (currentScene_){
		currentScene_->CleanUp();
		currentScene_.reset();
	}
}

//===================================================================*/
// 初期シーンの生成・初期化
//===================================================================*/
void SceneManager::Initialize(){
	// 初期シーンを生成
	std::unique_ptr<IScene> initialScene = std::make_unique<TestScene>(pDxCore_);

	// UIが必要ならセット
	if (pEngineUI_){
		initialScene->SetEngineUI(pEngineUI_);
	}

	// シーンマネージャをセット
	initialScene->SetSceneManager(this);

	// シーン初期化
	initialScene->Initialize();

	// 現在のシーンに設定
	currentScene_ = std::move(initialScene);
}

//===================================================================*/
// シーン切り替え予約があれば反映し、現在シーンを更新
//===================================================================*/
void SceneManager::Update(){
	// 次シーンの予約があれば切り替え
	if (nextScene_){
		SwitchToNextScene();
	}

	// シーンの更新
	if (currentScene_){
		currentScene_->Update();
	}
}

//===================================================================*/
// 現在のシーンを描画
//===================================================================*/
void SceneManager::Draw(){
	if (currentScene_){
		currentScene_->Draw();
	}
}

//===================================================================*/
// 次シーンへ切り替える内部処理
//===================================================================*/
void SceneManager::SwitchToNextScene(){
	// 旧シーンの終了
	if (currentScene_){
		currentScene_->CleanUp();
		currentScene_.reset();
	}

	// 次シーンを現在シーンへ
	currentScene_ = std::move(nextScene_);

	// シーンマネージャをセット
	currentScene_->SetSceneManager(this);

	// UIをセット
	if (pEngineUI_){
		currentScene_->SetEngineUI(pEngineUI_);
	}

	// シーン初期化
	currentScene_->Initialize();
}

#include "SceneManager.h"
#include "TestScene.h"
#include "TitleScene.h"
#include "GameScene.h"
#include "engine/objects/SceneObjectManager.h"

SceneManager::SceneManager(DxCore* dxCore)
    : pDxCore_(dxCore){
    // ここでシーンをすべて生成しておく場合
    scenes_[static_cast< int >(SceneType::TITLE)] = std::make_unique<TitleScene>(pDxCore_);
    scenes_[static_cast< int >(SceneType::PLAY)] = std::make_unique<GameScene>(pDxCore_);
    scenes_[static_cast< int >(SceneType::TEST)] = std::make_unique<TestScene>(pDxCore_);

    // 最初は TITLE シーンにしておく
    currentSceneNo_ = static_cast< int >(SceneType::PLAY);
    nextSceneNo_ = currentSceneNo_;
}

SceneManager::~SceneManager(){}

void SceneManager::Initialize(){
    // シーン切り替えパネルを作成 (UI がある場合)
    if (pEngineUI_){
        sceneSwitchPanel_ = std::make_unique<SceneSwitcherPanel>(this);

        sceneSwitchPanel_->AddSceneOption("Title Scene", SceneType::TITLE);
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
    // 現在のシーンを描画
    scenes_[currentSceneNo_]->Draw();
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

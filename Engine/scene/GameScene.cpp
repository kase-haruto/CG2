#include "GameScene.h"
#include"GlobalVariable/GlobalVariables.h"

GameScene::GameScene(){
	GlobalVariables::GetInstance()->Initialize();
}

void GameScene::Initialize(){

	///=========================
	/// カメラ関連
	///=========================
	viewProjection_ = std::make_unique<ViewProjection>();
	viewProjection_->Initialize();
	

	///=========================
	/// Editor関連
	///=========================
	railEditor_ = std::make_unique<RailEditor>();
	railEditor_->SetViewProjection(viewProjection_.get());
	railEditor_->Initialize();

}

void GameScene::Update(){

#ifdef _DEBUG
	viewProjection_->ImGui();
	GlobalVariables::GetInstance()->Update();
#endif // _DEBUG

	railEditor_->Update();

}

void GameScene::Draw(){
	railEditor_->Draw();
}

void GameScene::Finalize(){
	viewProjection_.reset();
	railEditor_.reset();
}

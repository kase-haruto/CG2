#include "DemoScene.h"

DemoScene::DemoScene(DxCore* dxCore) : IScene(dxCore){
	// シーン名を設定
	IScene::SetSceneName("testScene");
}

void DemoScene::Initialize(){
	///=========================
	/// グラフィック関連
	///=========================
	directionalLight_ = std::make_unique<DirectionalLight>();
	directionalLight_->Initialize(pDxCore_);

	pointLight_ = std::make_unique<PointLight>();
	pointLight_->Initialize(pDxCore_);

	fog_ = std::make_unique<FogEffect>(pDxCore_);


}

void DemoScene::Update(){}

void DemoScene::Draw(){}

void DemoScene::CleanUp(){}

void DemoScene::Draw3dObject(){}


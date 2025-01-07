#include "ResultScene.h"

#include "Engine/Collision/CollisionManager.h"
#include "SceneManager.h"
#include "Engine/core/Input.h"

ResultScene::ResultScene(DxCore* dxCore) : IScene(dxCore){
	// シーン名を設定
	IScene::SetSceneName("ResultScene");
}


void ResultScene::Initialize(){


	directionalLight_ = std::make_unique<DirectionalLight>();
	directionalLight_->Initialize(pDxCore_);

	pointLight_ = std::make_unique<PointLight>();
	pointLight_->Initialize(pDxCore_);

	fog_ = std::make_unique<FogEffect>(pDxCore_);
}

void ResultScene::Update(){

	if (Input::TriggerKey(DIK_SPACE)){
		pSceneManager_->RequestSceneChange(SceneType::TITLE);
	}

	CollisionManager::GetInstance()->UpdateCollisionAllCollider();
}

void ResultScene::Draw(){
	/////////////////////////////////////////////////////////////////////////////////////////
	//					3dオブジェクトの描画
	/////////////////////////////////////////////////////////////////////////////////////////
#pragma region 3Dオブジェクト描画
	//3dオブジェクト描画前処理
	ModelPreDraw();


#pragma endregion


	/////////////////////////////////////////////////////////////////////////////////////////
	//					2dオブジェクトの描画
	/////////////////////////////////////////////////////////////////////////////////////////
#pragma region 2Dオブジェクト描画
	//uiの描画

#pragma endregion
}

void ResultScene::CleanUp(){}

void ResultScene::ModelPreDraw(){
	pointLight_->Render();
	directionalLight_->Render();
	fog_->Update();
}

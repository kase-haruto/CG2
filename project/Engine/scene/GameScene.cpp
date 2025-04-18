/////////////////////////////////////////////////////////////////////////////////////////
//	include
/////////////////////////////////////////////////////////////////////////////////////////

// scene
#include "GameScene.h"
#include "SceneManager.h"

// engine
#include "../core/Input.h"
#include "Engine/core/Audio/Audio.h"
#include "../graphics/camera/CameraManager.h"
#include "Engine/objects/particle/ParticleManager.h"
#include "Engine/Collision/CollisionManager.h"
#include "Engine/core/DirectX/DxCore.h"
#include "Engine/physics/light/LightManager.h"


/////////////////////////////////////////////////////////////////////////////////////////
//	コンストラクタ/デストラクタ
/////////////////////////////////////////////////////////////////////////////////////////
GameScene::GameScene(){}

GameScene::GameScene(DxCore* dxCore) : IScene(dxCore){
	// シーン名を設定
	IScene::SetSceneName("GameScene");
}

/////////////////////////////////////////////////////////////////////////////////////////
//	初期化処理
/////////////////////////////////////////////////////////////////////////////////////////
void GameScene::Initialize(){
	CameraManager::GetInstance()->SetType(CameraType::Type_Debug);
	//=========================
	// グラフィック関連
	//=========================
	fog_ = std::make_unique<FogEffect>(pDxCore_);
	
	modelField_ = std::make_unique<Model>("ground.obj");
	modelField_->SetSize({100.0f,1.0f,100.0f});
	modelField_->SetUvScale({15.0f,15.0f,0.0f});

	//===================================================================*/
	//                    editor
	//===================================================================*/
}

void GameScene::Update(){
	LightManager::GetInstance()->ShowImGui();

	/* カメラ関連更新 ============================*/
	CameraManager::Update();
	
	/* 3dObject ============================*/
	//地面の更新
	modelField_->Update();
	
	/* その他 ============================*/

	CollisionManager::GetInstance()->UpdateCollisionAllCollider();
}

void GameScene::Draw(){
	/////////////////////////////////////////////////////////////////////////////////////////
	//					3dオブジェクトの描画
	/////////////////////////////////////////////////////////////////////////////////////////
#pragma region 3Dオブジェクト描画
	auto commandList_ = pDxCore_->GetCommandList();
	// light
	LightManager::GetInstance()->SetCommand(commandList_, LightType::Directional, PipelineType::Object3D);
	LightManager::GetInstance()->SetCommand(commandList_, LightType::Point, PipelineType::Object3D);
	// camera
	CameraManager::SetCommand(commandList_, PipelineType::Object3D);

	//地面の描画
	modelField_->Draw();

	PrimitiveDrawer::GetInstance()->Render();
#pragma endregion

	/////////////////////////////////////////////////////////////////////////////////////////
	//					2dオブジェクトの描画
	/////////////////////////////////////////////////////////////////////////////////////////
#pragma region 2Dオブジェクト描画

	
#pragma endregion
}

void GameScene::CleanUp(){
}


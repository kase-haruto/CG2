#include "GameScene.h"
/////////////////////////////////////////////////////////////////////////////////////////
//	include
/////////////////////////////////////////////////////////////////////////////////////////

// scene
#include <Engine/Scene/System/SceneManager.h>

// engine
#include <Engine/Application/Input/Input.h>
#include <Engine/Graphics/Camera/Manager/CameraManager.h>
#include <Engine/Objects/3D/Actor/SceneObjectManager.h>
#include <Engine/objects/particle/ParticleManager.h>
#include <Engine/Collision/CollisionManager.h>
#include <Engine/Graphics/Device/DxCore.h>
#include <Engine/Lighting/LightManager.h>


/////////////////////////////////////////////////////////////////////////////////////////
//	コンストラクタ/デストラクタ
/////////////////////////////////////////////////////////////////////////////////////////
GameScene::GameScene(){}

GameScene::GameScene(DxCore* dxCore) 
	: BaseScene(dxCore){
	// シーン名を設定
	//IScene::SetSceneName("GameScene");
	SetSceneName("GameScene");
}

/////////////////////////////////////////////////////////////////////////////////////////
//	初期化処理
/////////////////////////////////////////////////////////////////////////////////////////
void GameScene::Initialize(){
	CameraManager::GetInstance()->SetType(CameraType::Type_Default);
	//=========================
	// グラフィック関連
	//=========================
	railCamera_ = std::make_unique<RailCamera>();
	railCamera_->Initialize();
	
	modelField_ = std::make_unique<BaseGameObject>("terrain.obj","field");
	modelField_->SetScale({300.0f,300.0f,300.0f});
	modelField_->SetUvScale({290.0f,290.0f,0.0f});
	//modelField_->EnableGuiList();
	//modelField_->SetUvScale({15.0f,15.0f,0.0f});


	//player
	player_ = std::make_unique<Player>("player.obj");
	player_->Initialize();
	player_->SetParent(&railCamera_->GetWorldTransform());

	enemyCollection_ = std::make_unique<EnemyCollection>();

	
	//===================================================================*/
	//                    editor
	//===================================================================*/
}

void GameScene::Update(){
	LightManager::GetInstance()->ShowImGui();

	/* カメラ関連更新 ============================*/
	railCamera_->Update();
	CameraManager::GetCamera3d()->SetCamera(railCamera_->GetPosition(), railCamera_->GetRotation());
	CameraManager::Update();
	
	/* 3dObject ============================*/
	//地面の更新
	modelField_->Update();
	
	//プレイヤーの更新
	player_->Update();

	enemyCollection_->Update();
	/* その他 ============================*/

	CollisionManager::GetInstance()->UpdateCollisionAllCollider();
}


void GameScene::CleanUp(){
	// 3Dオブジェクトの描画を終了
	sceneContext_->GetMeshRenderer()->Clear();
}

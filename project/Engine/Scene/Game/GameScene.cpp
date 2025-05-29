#include "GameScene.h"
/////////////////////////////////////////////////////////////////////////////////////////
//	include
/////////////////////////////////////////////////////////////////////////////////////////

// scene
#include <Engine/Scene/System/SceneManager.h>
#include <Engine/Scene/Utirity/SceneUtility.h>

// engine
#include <Engine/Application/Input/Input.h>
#include <Engine/Graphics/Camera/Manager/CameraManager.h>
#include <Engine/Objects/3D/Actor/SceneObjectManager.h>
#include <Engine/Collision/CollisionManager.h>
#include <Engine/Graphics/Device/DxCore.h>
#include <Game/Effect/ParticleEffect/ParticleEffectCollection.h>

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
//	アセットのロード
/////////////////////////////////////////////////////////////////////////////////////////
void GameScene::LoadAssets() {
	ParticleEffectCollection* pfxCollection = ParticleEffectCollection::GetInstance();
	pfxCollection->Clear();

	// パーティクルエフェクトのロード
	pfxCollection->LoadByName("hitParticle");
	pfxCollection->LoadByName("reloadParticle");
	pfxCollection->LoadByName("shootEffect");
	pfxCollection->LoadByName("smoke");
	pfxCollection->LoadByName("BulletEffect");
}


/////////////////////////////////////////////////////////////////////////////////////////
//	初期化処理
/////////////////////////////////////////////////////////////////////////////////////////
void GameScene::Initialize(){
	LoadAssets();

	CameraManager::GetInstance()->SetType(CameraType::Type_Default);
	
	CreateAndAddObject<SkyBox>(sceneContext_.get(), skyBox_, "sky.dds", "skyBox");
	skyBox_->Initialize();

	//=========================
	// グラフィック関連
	//=========================
	railCamera_ = std::make_unique<RailCamera>();
	railCamera_->Initialize();
	
	CreateAndAddObject<BaseGameObject>(sceneContext_.get(), modelField_, "terrain.obj", "field");
	modelField_->SetScale({300.0f,300.0f,300.0f});

	//player
	CreateAndAddObject<Player>(sceneContext_.get(), player_, "player.obj", "player");
	player_->Initialize();
	player_->SetParent(&railCamera_->GetWorldTransform());

	enemyCollection_ = std::make_unique<EnemyCollection>();
	
	//===================================================================*/
	//                    editor
	//===================================================================*/
	sceneContext_->RegisterAllToRenderer();
}

void GameScene::Update(){


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

	sceneContext_->Update();
	CollisionManager::GetInstance()->UpdateCollisionAllCollider();
}


void GameScene::CleanUp(){
	// 3Dオブジェクトの描画を終了
	sceneContext_->GetMeshRenderer()->Clear();
	sceneContext_->GetObjectLibrary()->Clear();
	CollisionManager::GetInstance()->ClearColliders();
}

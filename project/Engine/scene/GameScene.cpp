#include "GameScene.h"

#include "../core/Input.h"
#include "Engine/core/Audio/Audio.h"

#include "../graphics/camera/CameraManager.h"

#include "Engine/objects/particle/ParticleManager.h"

#include "Engine/Collision/CollisionManager.h"

#include "Engine/core/DirectX/DxCore.h"

#include "SceneManager.h"

GameScene::GameScene(){}

GameScene::GameScene(DxCore* dxCore) : IScene(dxCore){
	// シーン名を設定
	IScene::SetSceneName("GameScene");
}

void GameScene::Initialize(){
	CameraManager::GetInstance()->SetType(CameraType::Type_Follow);
	///=========================
	/// グラフィック関連
	///=========================
	directionalLight_ = std::make_unique<DirectionalLight>();
	directionalLight_->Initialize(pDxCore_);

	pointLight_ = std::make_unique<PointLight>();
	pointLight_->Initialize(pDxCore_);

	fog_ = std::make_unique<FogEffect>(pDxCore_);

	////地面
	/*modelGround_ = std::make_unique<Model>("ground");
	modelGround_->SetViewProjection(viewProjection_.get());
	modelGround_->SetSize({100.0f,0.0f,100.0f});
	modelGround_->SetUvScale({30.0f,30.0f,0.0f});*/

	modelField_ = std::make_unique<Model>("ground.obj");
	modelField_->SetSize({100.0f,1.0f,100.0f});
	modelField_->SetUvScale({15.0f,15.0f,0.0f});

	player_ = std::make_unique<Player>("player.gltf");
	player_->Initialize();
	CameraManager::GetInstance()->SetFollowTarget(&player_->GetTransform());
	playerAttackEditor_ = std::make_unique<PlayerAttackEditor>(player_->GetAttackController());
	playerAttackEditor_->SetPlayer(player_.get());

	attackEditorPanel_ = std::make_unique<AttackEditorPanel>();
	attackEditorPanel_->SetPlayerAttackEditor(playerAttackEditor_.get());
	EngineUI::GetInstance()->AddPanel(std::move(attackEditorPanel_));

	particleEditPanel_ = std::make_unique<ParticleEditPanel>();
	particleEditPanel_->SetParticleManager(ParticleManager::GetInstance());
	EngineUI::GetInstance()->AddPanel(std::move(particleEditPanel_));


	enemyManager_ = std::make_unique<EnemyManager>();
	enemyManager_->SetTarget(&player_->GetPosition());
	enemyManager_->Initialize();

	/////////////////////////////////////////////////////////////////////////////////////////
	//							editor
	/////////////////////////////////////////////////////////////////////////////////////////
	//モデル
	modelBuilder_ = std::make_unique<ModelBuilder>();
	modelBuilder_->Initialize();

	//sprite
	uiEditor_ = std::make_unique<UIEditor>();

	Audio::Play("bgm.mp3", true,0.3f);


}

void GameScene::Update(){
#ifdef _DEBUG
#endif // _DEBUG

	pointLight_->SetPosition(player_->GetCenterPos());

	CameraManager::Update();

	//uiの更新
	uiEditor_->Update();

	//モデルの更新
	modelBuilder_->Update();

	modelField_->Update();

	player_->Update();

	enemyManager_->Update();

	CollisionManager::GetInstance()->UpdateCollisionAllCollider();

	/*modelGround_->Update();*/

	if (Input::TriggerKey(DIK_SPACE)){
		pSceneManager_->RequestSceneChange(SceneType::RESULT);
	}

}

void GameScene::Draw(){
	/////////////////////////////////////////////////////////////////////////////////////////
	//					3dオブジェクトの描画
	/////////////////////////////////////////////////////////////////////////////////////////
#pragma region 3Dオブジェクト描画
	//3dオブジェクト描画前処理
	ModelPreDraw();

	//モデルの描画
	modelBuilder_->Draw();

	player_->Draw();

	enemyManager_->Draw();

	/*modelGround_->Draw();*/

	playerAttackEditor_->Draw();

	//地面の描画
	modelField_->Draw();

	ParticleManager::GetInstance()->Draw();

	PrimitiveDrawer::GetInstance()->Render();


#pragma endregion


	/////////////////////////////////////////////////////////////////////////////////////////
	//					2dオブジェクトの描画
	/////////////////////////////////////////////////////////////////////////////////////////
#pragma region 2Dオブジェクト描画
	//uiの描画
	uiEditor_->Draw();

#pragma endregion
}

void GameScene::CleanUp(){
	EngineUI::GetInstance()->RemovePanel("AttackEditor");
	EngineUI::GetInstance()->RemovePanel("ParticleEdit");

	Audio::EndAudio("bgm.mp3");
}

void GameScene::ModelPreDraw(){
	pointLight_->Render();
	directionalLight_->Render();
	fog_->Update();
}
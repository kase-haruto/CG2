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
	CameraManager::GetInstance()->SetRotate(Vector3(0.0f, 0.0f, 0.0f));
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


	// スポナー間の間隔と開始位置を設定
	const float spacing = 15.0f;  // スポナー同士の間隔（X軸方向）
	const float zOffset = 5.0f;  // Z軸方向のジグザグ間隔
	Vector3 startPosition(-spacing * (maxEnemySpawners_ - 1) / 2.0f, 0.0f, 0.0f); // 中央揃えの開始位置


	enemySpawners_.resize(maxEnemySpawners_);
	for (size_t i = 0; i < maxEnemySpawners_; ++i){
		// スポナーの生成
		enemySpawners_[i] = std::make_unique<EnemySpawner>("enemySpawner.obj");

		// スポナーの位置を計算
		float zOffsetValue = (i % 2 == 0) ? zOffset : -zOffset; // 偶数番目は+zOffset、奇数番目は-zOffset
		Vector3 position = startPosition + Vector3(i * spacing, 0.0f, zOffsetValue);

		// スポナーの初期化
		enemySpawners_[i]->Initialize(position);

		// EnemyManager を設定（enemyManager_ は既に生成されている前提）
		enemySpawners_[i]->SetEnemyManager(enemyManager_.get());

	}

	for (size_t i = 0; i <= 5; i++){
		std::string textureName = std::to_string(i) + ".png";
		numSprites_[i] = std::make_unique<Sprite>(textureName);
		numSprites_[i]->Initialize({1180.0f,50.0f}, {50.0f,50.0f});
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	//							editor
	/////////////////////////////////////////////////////////////////////////////////////////
	//モデル
	modelBuilder_ = std::make_unique<ModelBuilder>();
	modelBuilder_->Initialize();

	//sprite
	uiEditor_ = std::make_unique<UIEditor>();

	//skydome
	skydome_ = std::make_unique<Model>("skydome.obj");
	skydome_->SetSize({70.0f,70.0f,70.0f});

	Audio::Play("bgm.mp3", true, 0.0f);

}

void GameScene::Update(){
	/* カメラ関連更新 ============================*/
	CameraManager::Update();

	/* player関連更新 ===================================*/
	player_->Update();

	/* 敵関連更新 ============================*/
	enemyManager_->Update();
	// スポナーの更新と無効化されたスポナーの削除
	enemySpawners_.erase(
		std::remove_if(
		enemySpawners_.begin(),
		enemySpawners_.end(),
		[] (const std::unique_ptr<EnemySpawner>& spawner){
			// GetIsActive() が false なら削除
			return !spawner->IsActive();
		}),
		enemySpawners_.end()
	);
	for (auto& spawner : enemySpawners_){
		spawner->Update();
	}

	spawnIndex_ = static_cast< int32_t >(enemySpawners_.size());

	/* その他 ============================*/
	//uiの更新
	uiEditor_->Update();
	uiEditor_->ShowImGuiInterface();
	//モデルの更新
	modelBuilder_->Update();
	//地面の更新
	modelField_->Update();

	skydome_->Update();

	//spriteの更新
	for (auto& sprite : numSprites_){
		sprite->Update();
	}

	CollisionManager::GetInstance()->UpdateCollisionAllCollider();

	if (spawnIndex_ <= 0){
		pSceneManager_->gameResult_ = true;
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


	player_->Draw();

	enemyManager_->Draw();
	for (auto& spawner : enemySpawners_){
		spawner->Draw();
	}

	//モデルの描画
	modelBuilder_->Draw();

	playerAttackEditor_->Draw();

	//地面の描画
	modelField_->Draw();

	skydome_->Draw();

	PrimitiveDrawer::GetInstance()->Render();
	ParticleManager::GetInstance()->Draw();



#pragma endregion


	/////////////////////////////////////////////////////////////////////////////////////////
	//					2dオブジェクトの描画
	/////////////////////////////////////////////////////////////////////////////////////////
#pragma region 2Dオブジェクト描画

	//if (spawnIndex_ >= 1 && spawnIndex_ <= 5){ // 1～5の範囲でチェック
	//	numSprites_[spawnIndex_]->Draw();  // spawnIndex_ を 1減算して対応
	//}

	//for (auto& spawner:enemySpawners_){
	//	spawner->DrawUI();
	//}

	////uiの描画
	//uiEditor_->Draw();

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
	CameraManager::TransfarToGPU();
}
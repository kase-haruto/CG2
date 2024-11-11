#include "GameScene.h"
#include"GlobalVariable/GlobalVariables.h"
#include "myFunc/PrimitiveDrawer.h"
#include "core/Input.h"
#include "objects/TextureManager.h"

#include "Collision/CollisionManager.h"

uint32_t GameScene::newScore_ = 0;

GameScene::GameScene(){
	GlobalVariables::GetInstance()->Initialize();
}

static Vector3 originPos;
static Vector3 originRotate;

void GameScene::Initialize(){
	//スコアのリセット
	score_ = 0;

	///=========================
	/// カメラ関連
	///=========================
	viewProjection_ = std::make_unique<ViewProjection>();
	viewProjection_->Initialize();
	originPos = viewProjection_->transform.translate;
	originRotate = viewProjection_->transform.rotate;

	///=========================
	/// 線の描画
	///=========================
	PrimitiveDrawer::GetInstance()->SetViewProjection(viewProjection_.get());
	PrimitiveDrawer::GetInstance()->Initialize();


	/////////////////////////////////////////////////////////////////////////////////////////
	//				天球
	/////////////////////////////////////////////////////////////////////////////////////////
	skydome_ = std::make_unique<Model>("skydome");
	skydome_->Initialize();
	skydome_->SetViewProjection(viewProjection_.get());
	skydome_->transform.scale = {100.0f,100.0f,100.0f};

	///=========================
	/// Editor関連
	///=========================
	railEditor_ = std::make_unique<RailEditor>();
	railEditor_->SetViewProjection(viewProjection_.get());
	railEditor_->Initialize();

	

	/////////////////////////////////////////////////////////////////////////////////////////
	//				レール
	/////////////////////////////////////////////////////////////////////////////////////////
	rail_.SetCtrlPoints(railEditor_->GetControlPoint());

	railCamera_ = std::make_unique<RailCamera>();
	railCamera_->SetCtrlPoints(railEditor_->GetControlPoint());

	/////////////////////////////////////////////////////////////////////////////////////////
	//				プレイヤー
	/////////////////////////////////////////////////////////////////////////////////////////
	playerModel_ = std::make_unique<Model>("suzanne");

	player_ = std::make_unique<Player>();
	player_->Initialize(playerModel_.get());
	player_->SetViewProjection(viewProjection_.get());

	player_->SetParent(&railCamera_->GetTransform());

	/////////////////////////////////////////////////////////////////////////////////////////
	//				敵
	/////////////////////////////////////////////////////////////////////////////////////////
	enemyManager_ = std::make_unique<EnemyManager>();
	enemyManager_->SetViewProjection(viewProjection_.get());



	/////////////////////////////////////////////////////////////////////////////////////////
	//				スコア
	/////////////////////////////////////////////////////////////////////////////////////////
	scoreSprites_.resize(5);
	for (size_t i = 0; i < 5; ++i){
		// 指定されたテクスチャ名でテクスチャをロード
		auto sprite = std::make_shared<Sprite>("0.png");  // 初期化時は "0" を使用
		Vector2 digitPosition = {-1000.0f, -1000.0f};  // 桁ごとの位置を設定
		sprite->Initialize(digitPosition, {64.0f, 64.0f});

		scoreSprites_[i] = sprite;
	}

	editor_ = std::make_unique<UIEditor>();
}

void GameScene::Update(){

#ifdef _DEBUG
	viewProjection_->ImGui();
	GlobalVariables::GetInstance()->Update();

	railEditor_->Update();
	rail_.SetCtrlPoints(railEditor_->GetControlPoint());

	ImGui::Begin("rail");

	editor_->ShowImGuiInterface();

	ImGui::End();

#endif // _DEBUG
	//カメラの更新
	viewProjection_->transform.translate = railCamera_->GetTransform().translate;
	viewProjection_->transform.rotate = railCamera_->GetTransform().rotate;
	viewProjection_->UpdateMatrix();

	UpdateScore();
	editor_->Update();
	//fieldの更新
	skydome_->Update();


	for (auto& enemy:enemyManager_->GetEnemies()){
		CollisionManager::GetInstance()->CheckCollisionSphereToLine(player_->GetLine(), enemy.get());
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	//		カメラの更新
	/////////////////////////////////////////////////////////////////////////////////////////

	railCamera_->Update();

	//playerの更新
	player_->Update();

	// スポーンタイマーをカウント
	spawnTime_++;
	if (spawnTime_ >= spawnTimeLimit_){
		// AABB空間の範囲設定
		Vector3 playerPosition = player_->GetWorldPosition();
		float spawnRange = 15.0f; // AABBの半径

		// プレイヤーの前方ベクトルを取得
		Vector3 forwardVector = player_->GetForwardVector();
		forwardVector.Normalize(); // 正規化して方向ベクトルとする

		// AABB内のランダムな位置を生成
		float randomX = (rand() / static_cast< float >(RAND_MAX)) * 2 * spawnRange - spawnRange;
		float randomY = (rand() / static_cast< float >(RAND_MAX)) * 2 * spawnRange - spawnRange;
		float randomZ = (rand() / static_cast< float >(RAND_MAX)) * 2 * spawnRange - spawnRange;

		Vector3 spawnPosition = playerPosition + Vector3(randomX, randomY, randomZ);

		// スポーン位置が前方でない場合は、前方ベクトルを加えて前方に位置を補正
		if ((spawnPosition - playerPosition).Dot(forwardVector) < 0){
			// 後方ならば前方に移動させる
			spawnPosition = playerPosition + forwardVector * spawnRange;
		}

		// 敵を追加
		enemyManager_->AddEnemy(spawnPosition);

		// タイマーをリセット
		spawnTime_ = 0;
	}

	//敵の更新
	enemyManager_->Update();
}

void GameScene::Draw(){
	

	skydome_->Draw();

	railEditor_->Draw();

	//playerの描画
	player_->Draw();
	//敵の描画
	enemyManager_->Draw();

	//線の描画
	PrimitiveDrawer::GetInstance()->Render();

	DrawScore();

	//spriteの描画
	editor_->Draw();

}

void GameScene::Finalize(){
	viewProjection_.reset();
	railEditor_.reset();
	player_.reset();
	enemyManager_.reset();
	playerModel_.reset();
	railCamera_.reset();
	PrimitiveDrawer::GetInstance()->Finalize();
}

void GameScene::UpdateScore(){
	// スコアが変わった場合にのみ更新
	if (score_ != newScore_){
		score_ = newScore_;
		// スコアが 0 の場合は特別に "0" を設定
		std::string scoreString = (score_ == 0) ? "0" : std::to_string(score_);

		// 桁数をスコアの桁数に合わせて設定
		for (size_t i = 0; i < scoreSprites_.size(); ++i){
			if (i < scoreString.size()){
				// 現在の桁の数字に対応するテクスチャを設定
				std::string textureFileName = std::string(1, scoreString[i]) + ".png";
				// 指定されたテクスチャ名でテクスチャをロード
				D3D12_GPU_DESCRIPTOR_HANDLE textureHandle = TextureManager::GetInstance()->LoadTexture(textureFileName);

				scoreSprites_[i]->SetTextureHandle(textureHandle);

				// スプライトの位置を更新（表示される桁のみ）
				Vector2 digitPosition = {1000.0f + i * 32.0f, 600.0f};
				scoreSprites_[i]->SetPosition(digitPosition);
			} else{
				// スコアの桁数より多い場合は、非表示状態にする
				scoreSprites_[i]->SetPosition({-1000.0f, -1000.0f});  // 画面外に移動
			}
		}
	}

	// スプライトの更新
	for (const auto& sprite : scoreSprites_){
		if (sprite){
			sprite->Update();
		}
	}
}



void GameScene::DrawScore(){
	// すべてのスプライトを描画
	for (const auto& sprite : scoreSprites_){
		if (sprite){
			sprite->Draw();
		}
	}
}


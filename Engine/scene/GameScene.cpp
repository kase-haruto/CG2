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

GameScene::~GameScene(){ boss_.release(); }

static Vector3 originPos;
static Vector3 originRotate;

void GameScene::Initialize(){
	// スコアのリセット
	score_ = 0;

	// カメラ関連
	viewProjection_ = std::make_unique<ViewProjection>();
	if (viewProjection_){
		viewProjection_->Initialize();
		originPos = viewProjection_->transform.translate;
		originRotate = viewProjection_->transform.rotate;
	}

	// 線の描画
	auto primitiveDrawer = PrimitiveDrawer::GetInstance();
	if (primitiveDrawer && viewProjection_){
		primitiveDrawer->SetViewProjection(viewProjection_.get());
		primitiveDrawer->Initialize();
	}

	// 天球
	skydome_ = std::make_unique<Model>("skydome");
	if (skydome_ && viewProjection_){
		skydome_->SetViewProjection(viewProjection_.get());
		skydome_->transform.scale = {800.0f, 800.0f, 800.0f};
	}

	ground_ = std::make_unique<Model>("ground");
	if (ground_ && viewProjection_){
		ground_->materialParameter.shininess = 50.0f;
		ground_->SetViewProjection(viewProjection_.get());
		ground_->transform.scale = {800.0f, 400.0f, 800.0f};
		ground_->transform.translate = {0.0f, -200.0f, 0.0f};
	}

	// 他の初期化部分にも同様にnullptrチェックを追加
	railEditor_ = std::make_unique<RailEditor>();
	if (railEditor_ && viewProjection_){
		railEditor_->SetViewProjection(viewProjection_.get());
		railEditor_->Initialize();
	}

	railCamera_ = std::make_unique<RailCamera>();
	if (railCamera_ && railEditor_){
		railCamera_->SetCtrlPoints(railEditor_->GetControlPoint());
		railCamera_->Update();
	}

	playerModel_ = std::make_unique<Model>("suzanne");
	player_ = std::make_unique<Player>();
	if (player_ && playerModel_ && viewProjection_){
		player_->Initialize(playerModel_.get());
		player_->SetViewProjection(viewProjection_.get());
		player_->SetParent(&railCamera_->GetTransform());
	}

	enemyManager_ = std::make_unique<EnemyManager>();
	if (enemyManager_ && viewProjection_){
		enemyManager_->SetViewProjection(viewProjection_.get());
	}

	bossModel_ = std::make_unique<Model>("ufo");
	boss_ = std::make_unique<Boss>();
	if (boss_ && bossModel_ && viewProjection_){
		boss_->Initialize(bossModel_.get(), {-50.0f, 22.0f, -45.0f});
		boss_->SetViewProjection(viewProjection_.get());
		boss_->Update();
	}

	// scoreSprites_とresultScoreSprites_の初期化
	scoreSprites_.resize(5);
	for (size_t i = 0; i < scoreSprites_.size(); ++i){
		auto sprite = std::make_shared<Sprite>("0.png");
		if (sprite){
			sprite->Initialize({-1000.0f, -1000.0f}, {64.0f, 64.0f});
			scoreSprites_[i] = sprite;
		}
	}

	resultScoreSprites_.resize(5);
	for (size_t i = 0; i < resultScoreSprites_.size(); ++i){
		auto sprite = std::make_shared<Sprite>("0.png");
		if (sprite){
			sprite->Initialize({640.0f, 360.0f}, {128.0f, 128.0f});
			sprite->SetAnchorPoint({0.5f, 0.5f});
			resultScoreSprites_[i] = sprite;
		}
	}

	// スプライトの初期化
	scoreFrame_ = std::make_unique<Sprite>("score.png");
	if (scoreFrame_){
		scoreFrame_->Initialize({900.0f, 500.0f}, {320.0f, 320.0f});
	}

	title_ = std::make_unique<Sprite>("title.png");
	if (title_){
		title_->Initialize({640.0f, 300.0f}, {1000.0f, 1000.0f});
		title_->SetAnchorPoint({0.5f, 0.5f});
	}

	space_ = std::make_unique<Sprite>("space.png");
	if (space_){
		space_->Initialize({640.0f, 500.0f}, {500.0f, 500.0f});
		space_->SetAnchorPoint({0.5f, 0.5f});
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

	editor_->Update();
	//fieldの更新
	skydome_->Update();
	ground_->Update();

	//現在の状態のシーンの描画
	switch (gameState_){
		case START:
			StartUpdate();
			break;

		case PLAY:
			PlayUpdate();
			break;

		case END:
			ResultUpdate();
			break;
	}

	
}

void GameScene::Draw(){

	skydome_->Draw();
	ground_->Draw();
	railEditor_->Draw();


	//現在の状態のシーンの描画
	switch (gameState_){
		case START:
			StartDraw();
			break;
		
		case PLAY:
			PlayDraw();
			break;

		case END:
			ResultDraw();
			break;
	}	

	//線の描画
	PrimitiveDrawer::GetInstance()->Render();

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
	// スコアを5桁のゼロパディング文字列に変換
	std::string scoreString = std::to_string(newScore_);
	while (scoreString.size() < 5){
		scoreString = "0" + scoreString;
	}

	// 各桁に対応するスプライトのテクスチャと位置を更新
	for (size_t i = 0; i < scoreSprites_.size(); ++i){
		// 現在の桁の数字に対応するテクスチャファイル名を取得
		std::string textureFileName = std::string(1, scoreString[i]) + ".png";

		// テクスチャハンドルをロードし設定
		D3D12_GPU_DESCRIPTOR_HANDLE textureHandle = TextureManager::GetInstance()->LoadTexture(textureFileName);
		scoreSprites_[i]->SetTextureHandle(textureHandle);

		// 各スプライトの位置を設定 (1000.0f + i * 32.0f, 600.0f は適宜調整可能)
		Vector2 digitPosition = {1050.0f + i * 25.0f, 600.0f};
		scoreSprites_[i]->SetPosition(digitPosition);
	}

	// スプライトの更新
	for (const auto& sprite : scoreSprites_){
		if (sprite){
			sprite->Update();
		}
	}

	scoreFrame_->Update();
}

void GameScene::DrawScore(){
	scoreFrame_->Draw();
	// すべてのスプライトを描画
	for (const auto& sprite : scoreSprites_){
		if (sprite){
			sprite->Draw();
		}
	}

}


/////////////////////////////////////////////////////////////////////////////////////////
//		状態ごとのsceneの更新
/////////////////////////////////////////////////////////////////////////////////////////

void GameScene::StartUpdate(){
	viewProjection_->transform.translate = railCamera_->GetTransform().translate;
	viewProjection_->transform.rotate = railCamera_->GetTransform().rotate;
	viewProjection_->UpdateMatrix();

	title_->Update();

	space_->Update();

	if (Input::TriggerKey(DIK_SPACE)){
		gameState_ = PLAY;
	}
}

void GameScene::PlayUpdate(){
	//カメラの更新
	viewProjection_->transform.translate = railCamera_->GetTransform().translate;
	viewProjection_->transform.rotate = railCamera_->GetTransform().rotate;
	viewProjection_->UpdateMatrix();

	UpdateScore();


	for (auto& enemy : enemyManager_->GetEnemies()){
		CollisionManager::GetInstance()->CheckCollisionSphereToLine(player_->GetLine(), enemy.get());
	}

	CollisionManager::GetInstance()->CheckCollisionSphereToLine(player_->GetLine(), boss_.get());
	/////////////////////////////////////////////////////////////////////////////////////////
	//		カメラの更新
	/////////////////////////////////////////////////////////////////////////////////////////

	railCamera_->Update();

	//playerの更新
	player_->Update();

	if (railCamera_->GetIsPaused()){
		boss_->SetIsSpawn(true);
	}

	if (boss_){

		boss_->Update();

	}

	// スポーンタイマーをカウント
	spawnTime_++;
	if (spawnTime_ >= spawnTimeLimit_){
		// AABB空間の範囲設定
		Vector3 playerPosition = player_->GetWorldPosition();
		float spawnRange = 40.0f; // AABBの半径

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

	//Railが終了したらリザルトsceneに移行
	if (railCamera_->GetIsFinished()){
		gameState_ = END;
	}
}

void GameScene::ResultUpdate(){
	viewProjection_->UpdateMatrix();

	space_->Update();

	// スプライトの更新
	for (const auto& sprite : resultScoreSprites_){
		if (sprite){
			sprite->Update();
		}
	}
	if (Input::TriggerKey(DIK_SPACE)){

		Reset();

		gameState_ = START;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
//		状態ごとのsceneの描画
/////////////////////////////////////////////////////////////////////////////////////////
void GameScene::StartDraw(){

	player_->Draw();

	title_->Draw();

	// フレームカウンターを用いたちらつき効果
	static int frameCounter = 0; // フレームカウンター
	frameCounter++;

	// フレーム数に応じて表示・非表示を切り替え
	if (frameCounter % 30 < 10){ // 5フレームごとに表示・非表示を切り替え
		space_->Draw();
	}
}

void GameScene::PlayDraw(){
	player_->Draw();
	
	//Railの描画
	railEditor_->Draw();

	//ボスの描画
	if (boss_){
		boss_->Draw();
	}

	//敵の描画
	enemyManager_->Draw();

	DrawScore();

	player_->DrawUi();
}

void GameScene::ResultDraw(){
	// スコアを画面中央に大きく表示
	std::string scoreString = std::to_string(newScore_);
	while (scoreString.size() < 5){
		scoreString = "0" + scoreString;
	}

	float centralX = 640.0f;
	float centralY = 360.0f;
	float largeSize = 128.0f;
	float totalWidth = largeSize * scoreString.size();

	for (size_t i = 0; i < resultScoreSprites_.size(); ++i){
		std::string textureFileName = std::string(1, scoreString[i]) + ".png";
		D3D12_GPU_DESCRIPTOR_HANDLE textureHandle = TextureManager::GetInstance()->LoadTexture(textureFileName);
		resultScoreSprites_[i]->SetTextureHandle(textureHandle);
		Vector2 digitPosition = {centralX - totalWidth / 2 + i * largeSize + largeSize / 2, centralY};
		resultScoreSprites_[i]->SetPosition(digitPosition);
		resultScoreSprites_[i]->SetSize({largeSize, largeSize});
		resultScoreSprites_[i]->Draw();
	}

	// フレームカウンターを用いたちらつき効果
	static int frameCounter = 0; // フレームカウンター
	frameCounter++;

	// フレーム数に応じて表示・非表示を切り替え
	if (frameCounter % 30 < 10){ // 5フレームごとに表示・非表示を切り替え
		space_->Draw();
	}
}


void GameScene::Reset(){

	//遷移する際にRailの終了フラグを初期化
	railCamera_->SetIsFinished(false);

	score_ = 0;

	newScore_ = 0;

	//敵のリストの初期化
	enemyManager_->Reset();

	boss_->Reset();

}


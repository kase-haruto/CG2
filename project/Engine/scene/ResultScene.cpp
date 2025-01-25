#include "ResultScene.h"

#include "Engine/Collision/CollisionManager.h"
#include "Engine/core/Clock/ClockManager.h"
#include "SceneManager.h"
#include "Engine/core/Input.h"
#include "Engine/core/System.h"

ResultScene::ResultScene(DxCore* dxCore) : IScene(dxCore){
	// シーン名を設定
	IScene::SetSceneName("ResultScene");
}


void ResultScene::Initialize(){

	fog_ = std::make_unique<FogEffect>(pDxCore_);

	//スプライト
	resultSprites_[0] = std::make_unique<Sprite>("clear.png");
	resultSprites_[0]->Initialize({0.0f,0.0f}, {1280.0f,720.0f});
	resultSprites_[1] = std::make_unique<Sprite>("defeate.png");
	resultSprites_[1]->Initialize({0.0f,0.0f}, {1280.0f,720.0f});

	startSprite_ = std::make_unique<Sprite>("startA.png");
	startSprite_->Initialize({640.0f,600.0f}, {100.0f,100.0f});
}

void ResultScene::Update(){
	startSprite_->Update();
	for (auto& resultSprite:resultSprites_){
		resultSprite->Update();
	}

	if (Input::TriggerGamepadButton(PAD_BUTTON::A)){
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
		//uiの描画
	static bool isVisible = true;        // 表示状態
	static float blinkTimer = 0.0f;      // タイマー
	const float blinkInterval = 1.0f;    // 点滅間隔（秒）

	// 時間の取得
	float deltaTime = ClockManager::GetInstance()->GetDeltaTime(); // フレーム間の経過時間を取得
	blinkTimer += deltaTime;

	// 1秒ごとに表示状態を切り替え
	if (blinkTimer >= blinkInterval){
		isVisible = !isVisible;
		blinkTimer = 0.0f; // タイマーをリセット
	}

	// 表示状態が true のときのみスプライトを描画
	if (isVisible){
		startSprite_->Draw();
	}
	//スプライト
	if (pSceneManager_->gameResult_){
		resultSprites_[0]->Draw();
	} else{
		resultSprites_[1]->Draw();
	}

#pragma endregion
}

void ResultScene::CleanUp(){}

void ResultScene::ModelPreDraw(){
	fog_->Update();
}

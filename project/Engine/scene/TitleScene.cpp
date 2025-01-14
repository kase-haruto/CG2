#include "TitleScene.h"
#include "../graphics/camera/CameraManager.h"
#include "Engine/core/Clock/ClockManager.h"
#include "Engine/Collision/CollisionManager.h"
#include "SceneManager.h"
#include "Engine/core/Input.h"
#include "Engine/core/System.h"

TitleScene::TitleScene(DxCore* dxCore) : IScene(dxCore){
	// シーン名を設定
	IScene::SetSceneName("TitleScene");
}


void TitleScene::Initialize(){
	CameraManager::GetInstance()->SetType(CameraType::Type_Default);

	directionalLight_ = std::make_unique<DirectionalLight>();
	directionalLight_->Initialize(pDxCore_);

	pointLight_ = std::make_unique<PointLight>();
	pointLight_->Initialize(pDxCore_);

	fog_ = std::make_unique<FogEffect>(pDxCore_);

	//sprite

	titleSprite_ = std::make_unique<Sprite>("title.png");
	titleSprite_->Initialize({0.0f,0.0f}, {1280.0f,720.0f});

	startSprite_ = std::make_unique<Sprite>("startA.png");
	startSprite_->Initialize({640.0f,600.0f}, {100.0f,100.0f});

}

void TitleScene::Update(){
	startSprite_->Update();
	titleSprite_->Update();
	if (Input::TriggerGamepadButton(PAD_BUTTON::A)){
		pSceneManager_->RequestSceneChange(SceneType::PLAY);
	}

	CollisionManager::GetInstance()->UpdateCollisionAllCollider();
}

void TitleScene::Draw(){
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

	titleSprite_->Draw();
#pragma endregion
}

void TitleScene::CleanUp(){}

void TitleScene::ModelPreDraw(){
	pointLight_->Render();
	directionalLight_->Render();
	fog_->Update();
}

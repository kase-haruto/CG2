#include "TitleScene.h"
/////////////////////////////////////////////////////////////////////////////////////////
//	include
/////////////////////////////////////////////////////////////////////////////////////////

// scene
#include <Engine/Scene/System/SceneManager.h>
#include <Engine/Scene/Utirity/SceneUtility.h>

// engine
#include <Engine/Graphics/Context/GraphicsGroup.h>
#include <Engine/Application/Input/Input.h>
#include <Engine/Graphics/Camera/Manager/CameraManager.h>
#include <Engine/Objects/3D/Actor/SceneObjectManager.h>
#include <Engine/Collision/CollisionManager.h>

/////////////////////////////////////////////////////////////////////////////////////////
//	コンストラクタ/デストラクタ
/////////////////////////////////////////////////////////////////////////////////////////
TitleScene::TitleScene() {
	// シーン名を設定
	//IScene::SetSceneName("TitleScene");
	SetSceneName("TitleScene");

}

/////////////////////////////////////////////////////////////////////////////////////////
//	アセットのロード
/////////////////////////////////////////////////////////////////////////////////////////
void TitleScene::LoadAssets() {}


/////////////////////////////////////////////////////////////////////////////////////////
//	初期化処理
/////////////////////////////////////////////////////////////////////////////////////////
void TitleScene::Initialize() {
	sceneContext_->Initialize();
	LoadAssets();

	CameraManager::GetInstance()->SetType(CameraType::Type_Default);

	//=========================
	// グラフィック関連
	//=========================
	modelField_ = sceneContext_->GetObjectLibrary()->CreateAndAddObject<BaseGameObject>("terrain.obj", "field");
	modelField_->SetScale({ 300.0f,300.0f,300.0f });

}

void TitleScene::Update() {
	/* カメラ関連更新 ============================*/
	CameraManager::Update();

	skyBox_->Update();

	/* 3dObject ============================*/
	//地面の更新
	modelField_->Update();

	/* その他 ============================*/
	sceneContext_->Update();
	CollisionManager::GetInstance()->UpdateCollisionAllCollider();

	if (Input::GetInstance()->TriggerKey(DIK_2)) {
		if (transitionRequestor_) {
			transitionRequestor_->RequestSceneChange(SceneType::PLAY);
		}
	}
}


void TitleScene::CleanUp() {
	// 3Dオブジェクトの描画を終了
	sceneContext_->GetObjectLibrary()->Clear();
	CollisionManager::GetInstance()->ClearColliders();
}


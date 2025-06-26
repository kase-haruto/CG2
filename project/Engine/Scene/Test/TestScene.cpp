/////////////////////////////////////////////////////////////////////////////////////////
//	include
/////////////////////////////////////////////////////////////////////////////////////////

// scene
#include <Engine/Scene/Test/TestScene.h>
#include <Engine/Scene/Utirity/SceneUtility.h>

// engine
#include <Engine/Application/Input/Input.h>
#include <Engine/Application/UI/Panels/EditorPanel.h>
#include <Engine/Collision/CollisionManager.h>
#include <Engine/Graphics/Camera/Manager/CameraManager.h>
#include <Engine/Graphics/Context/GraphicsGroup.h>
#include <Engine/Graphics/Device/DxCore.h>
#include <Engine/Objects/3D/Actor/SceneObjectManager.h>
#include <Engine/Application/Effects/FxSystem.h>

// lib
#include <Engine/Foundation/Utility/Func/MyFunc.h>

/////////////////////////////////////////////////////////////////////////////////////////
//	コンストラクタ/デストラクタ
/////////////////////////////////////////////////////////////////////////////////////////
TestScene::TestScene() {}

TestScene::TestScene(DxCore* dxCore)
	: BaseScene(dxCore) {
	// シーン名を設定
	BaseScene::SetSceneName("TestScene");

}

/////////////////////////////////////////////////////////////////////////////////////////
//	アセットのロード
/////////////////////////////////////////////////////////////////////////////////////////
void TestScene::LoadAssets() {
}

/////////////////////////////////////////////////////////////////////////////////////////
//	初期化処理
/////////////////////////////////////////////////////////////////////////////////////////
void TestScene::Initialize() {

	sceneContext_->SetSceneName("TestScene");

	LoadAssets();

	CameraManager::GetInstance()->SetType(CameraType::Type_Debug);
	//=========================
	// グラフィック関連
	//=========================

	skyBox_ = std::make_unique<SkyBox>("sky.dds", "skyBox");
	skyBox_->Initialize();
	sceneContext_->GetMeshRenderer()->SetSkyBox(skyBox_.get());

	//=========================
	// オブジェクト生成
	//=========================
	//CreateAndAddObject<BaseGameObject>(sceneContext_.get(), field_, "ground.obj", "field");
	//field_->SetScale({ 100.0f, 1.0f, 100.0f });
	//field_->SetEnableRaycast(false);

	//CreateAndAddObject<BaseGameObject>(sceneContext_.get(), bunny_, "bunny.obj", "bunny");
	//bunny_->SetTranslate({-10.0f, 0.0f, 0.0f});

	//CreateAndAddObject<BaseGameObject>(sceneContext_.get(),teapot_,"debugSphere.obj", "enviromentSphere");
	//teapot_->SetTranslate({5.0f, 0.0f, 0.0f});

	//CreateAndAddObject<BaseGameObject>(sceneContext_.get(),walkHuman_,"sneakWalk.gltf", "human");
	//walkHuman_->SetColor({1.0f, 1.0f, 1.0f, 0.5f});

	CreateAndAddObject<ParticleSystemObject>(sceneContext_.get(), particleSystem_, "particleSystem");
	sceneContext_->GetFxSystem()->AddEmitter(particleSystem_.get());

	CreateAndAddObject<ParticleSystemObject>(sceneContext_.get(), particleSystem2_, "particleSystem");
	sceneContext_->GetFxSystem()->AddEmitter(particleSystem2_.get());

	//=========================
	// 描画登録
	//=========================
	sceneContext_->RegisterAllToRenderer();

}

/////////////////////////////////////////////////////////////////////////////////////////
//	更新処理
/////////////////////////////////////////////////////////////////////////////////////////
void TestScene::Update() {

	CameraManager::Update();

	skyBox_->Update();

	//test
	//field_->Update();
	//bunny_->Update();
	//teapot_->Update();
	//walkHuman_->Update();

	//衝突判定
	CollisionManager::GetInstance()->UpdateCollisionAllCollider();

	sceneContext_->Update();
}


void TestScene::CleanUp() {
	// 3Dオブジェクトの描画を終了
	sceneContext_->GetMeshRenderer()->Clear();
	sceneContext_->GetObjectLibrary()->Clear();
	CollisionManager::GetInstance()->ClearColliders();
}




/////////////////////////////////////////////////////////////////////////////////////////
//	include
/////////////////////////////////////////////////////////////////////////////////////////

// scene
#include <Engine/Scene/Test/TestScene.h>
#include <Engine/Scene/Utirity/SceneUtility.h>

// engine
#include <Engine/Application/Input/Input.h>
#include <Engine/Collision/CollisionManager.h>
#include <Engine/Graphics/Camera/Manager/CameraManager.h>
#include <Engine/Foundation/Utility/Func/MyFunc.h>
#include <Engine/Graphics/Pipeline/Service/PipelineService.h>

// lib

/////////////////////////////////////////////////////////////////////////////////////////
//	コンストラクタ/デストラクタ
/////////////////////////////////////////////////////////////////////////////////////////
TestScene::TestScene(){
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

	sceneContext_->Initialize();
	sceneContext_->SetSceneName("TestScene");


	LoadAssets();

	CameraManager::GetInstance()->SetType(CameraType::Type_Debug);
	//=========================
	// グラフィック関連
	//=========================

	skyBox_ = std::make_unique<SkyBox>("sky.dds", "skyBox");
	skyBox_->Initialize();

	//=========================
	// オブジェクト生成
	//=========================
	testObject = sceneContext_->GetObjectLibrary()->CreateAndAddObject<BaseGameObject>("debugCube.obj");
	testSprite_ = std::make_unique<Sprite>("Textures/uvChecker.png");
	Vector3 wPos = testObject->GetWorldPosition();
	Vector2 pos = WorldToScreen(wPos);
	testSprite_->Initialize(pos, Vector2(32.0f, 32.0f));
	testSprite_->SetAnchorPoint(Vector2(0.5f, 0.5f));

}

/////////////////////////////////////////////////////////////////////////////////////////
//	更新処理
/////////////////////////////////////////////////////////////////////////////////////////
void TestScene::Update() {

	CameraManager::Update();

	skyBox_->Update();

	testObject->Update();

	Vector3 wPos = testObject->GetWorldPosition();
	Vector2 pos = WorldToScreen(wPos);
	testSprite_->SetPosition(pos);
	testSprite_->Update();

	//衝突判定
	CollisionManager::GetInstance()->UpdateCollisionAllCollider();

	sceneContext_->Update();
}

void TestScene::Draw(ID3D12GraphicsCommandList* cmdList,  PipelineService* psoService) {
	BaseScene::Draw(cmdList,psoService);
	testSprite_->Draw(cmdList);
}

void TestScene::CleanUp() {
	// 3Dオブジェクトの描画を終了
	sceneContext_->GetObjectLibrary()->Clear();
	CollisionManager::GetInstance()->ClearColliders();
}




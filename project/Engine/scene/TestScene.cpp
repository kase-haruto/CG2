/////////////////////////////////////////////////////////////////////////////////////////
//	include
/////////////////////////////////////////////////////////////////////////////////////////

// scene
#include "../scene/TestScene.h"

// engine
#include "Engine/core/Input.h"
#include "Engine/core/Audio/Audio.h"
#include "Engine/graphics/camera/CameraManager.h"
#include "Engine/objects/particle/ParticleManager.h"
#include "Engine/Collision/CollisionManager.h"
#include "Engine/core/DirectX/DxCore.h"
#include "Engine/objects/SceneObjectManager.h"
#include "Engine/core/UI/EditorPanel.h"
#include "Engine/physics/light/LightManager.h"
#include "Engine/graphics/GraphicsGroup.h"

// lib
#include "lib/myFunc/MyFunc.h"

/////////////////////////////////////////////////////////////////////////////////////////
//	コンストラクタ/デストラクタ
/////////////////////////////////////////////////////////////////////////////////////////
TestScene::TestScene(){}

TestScene::TestScene(DxCore* dxCore) 
	: BaseScene(dxCore){
	// シーン名を設定
	BaseScene::SetSceneName("TestScene");
}

/////////////////////////////////////////////////////////////////////////////////////////
//	初期化処理
/////////////////////////////////////////////////////////////////////////////////////////
void TestScene::Initialize(){
	auto registerToRenderer = [this] (IMeshRenderable* mesh){
		sceneContext_->meshRenderer_->Register(mesh);
		};

	CameraManager::GetInstance()->SetType(CameraType::Type_Debug);
	//=========================
	// グラフィック関連
	//=========================
	fog_ = std::make_unique<FogEffect>(pDxCore_);

	skyBox_ = std::make_unique<SkyBox>("rostock_laage_airport_4k.dds",registerToRenderer);
	skyBox_->Initialize();

	//objects
	modelField_ = std::make_unique<Model>("ground.obj");
	modelField_->SetSize({100.0f,1.0f,100.0f});
	modelField_->SetUvScale({15.0f,15.0f,0.0f});
	sceneContext_->meshRenderer_->Register(modelField_.get());

	//test用
	bunny_ = std::make_unique<BaseGameObject>("bunny.obj","bunny", registerToRenderer);
	bunny_->SetTranslate({-10.0f, 0.0f, 0.0f});

	teapot_ = std::make_unique<BaseGameObject>("teapot.obj","teapot", registerToRenderer);
	teapot_->SetTranslate({5.0f, 0.0f, 0.0f});

	walkHuman_ = std::make_unique<BaseGameObject>("sneakWalk.gltf","human", registerToRenderer);
	walkHuman_->SetColor({1.0f, 1.0f, 1.0f, 0.5f});

	/////////////////////////////////////////////////////////////////////////////////////////
	//							editor
	/////////////////////////////////////////////////////////////////////////////////////////

}

/////////////////////////////////////////////////////////////////////////////////////////
//	更新処理
/////////////////////////////////////////////////////////////////////////////////////////
void TestScene::Update(){
#ifdef _DEBUG

	Input::ShowImGui();

#endif //  _DEBUG

	CameraManager::Update();

	//地面
	//modelField_->Update();

	skyBox_->Update({ 0.0f, 0.0f, 0.0f }, { 20.0f, 20.0f, 20.0f });

	//test
	bunny_->Update();
	teapot_->Update();
	walkHuman_->Update();

	//衝突判定
	CollisionManager::GetInstance()->UpdateCollisionAllCollider();

	PrimitiveDrawer::GetInstance()->DrawGrid();
}


void TestScene::CleanUp(){
	// 3Dオブジェクトの描画を終了
	sceneContext_->meshRenderer_->Clear();
	SceneObjectManager::GetInstance()->ClearAllObject();
}


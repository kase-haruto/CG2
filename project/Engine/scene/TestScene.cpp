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

TestScene::TestScene(DxCore* dxCore) : IScene(dxCore){
	// シーン名を設定
	IScene::SetSceneName("testScene");
}

/////////////////////////////////////////////////////////////////////////////////////////
//	初期化処理
/////////////////////////////////////////////////////////////////////////////////////////
void TestScene::Initialize(){
	CameraManager::GetInstance()->SetType(CameraType::Type_Debug);
	//=========================
	// グラフィック関連
	//=========================
	fog_ = std::make_unique<FogEffect>(pDxCore_);

	//objects
	modelField_ = std::make_unique<Model>("ground.obj");
	modelField_->SetSize({100.0f,1.0f,100.0f});
	modelField_->SetUvScale({15.0f,15.0f,0.0f});

	//test用
	bunny_ = std::make_unique<BaseGameObject>("bunny.obj");
	bunny_->SetName("bunny");
	bunny_->SetTranslate({-10.0f, 0.0f, 0.0f});
	teapot_ = std::make_unique<BaseGameObject>("teapot.obj");
	teapot_->SetName("teapot");
	teapot_->SetTranslate({5.0f, 0.0f, 0.0f});

	walkHuman_ = std::make_unique<AnimationModel>("sneakWalk.gltf");
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

	//test
	bunny_->Update();
	teapot_->Update();
	walkHuman_->AnimationUpdate();

	//衝突判定
	CollisionManager::GetInstance()->UpdateCollisionAllCollider();
}

void TestScene::Draw(){
	/////////////////////////////////////////////////////////////////////////////////////////
	//					2dオブジェクトの描画
	/////////////////////////////////////////////////////////////////////////////////////////
#pragma region 2Dオブジェクト背景描画

#pragma endregion

	/////////////////////////////////////////////////////////////////////////////////////////
	//					3dオブジェクトの描画
	/////////////////////////////////////////////////////////////////////////////////////////
#pragma region 3Dオブジェクト描画

	Draw3dObject();

#pragma endregion

	/////////////////////////////////////////////////////////////////////////////////////////
	//					2dオブジェクトの描画
	/////////////////////////////////////////////////////////////////////////////////////////
#pragma region 2Dオブジェクト描画

#pragma endregion
}

void TestScene::CleanUp(){
	SceneObjectManager::GetInstance()->ClearAllObject();
}

/////////////////////////////////////////////////////////////////////////////////////////
//					3dオブジェクト描画
/////////////////////////////////////////////////////////////////////////////////////////
void TestScene::Draw3dObject(){
	/*=======================================================================================
				モデルの描画
	========================================================================================*/
	auto commandList_ = pDxCore_->GetCommandList();
	// light
	LightManager::GetInstance()->SetCommand(commandList_, LightType::Directional, PipelineType::Object3D);
	LightManager::GetInstance()->SetCommand(commandList_, LightType::Point, PipelineType::Object3D);
	// camera
	CameraManager::SetCommand(commandList_, PipelineType::Object3D);

	//地面の描画
	modelField_->Draw();

	//test
	bunny_->Draw();
	teapot_->Draw();
	walkHuman_->Draw();

	/* =======================================================================================
				particleの描画
	========================================================================================*/

	/* =======================================================================================
				プリミティブな図形の描画
	========================================================================================*/
	PrimitiveDrawer::GetInstance()->DrawGrid();
	PrimitiveDrawer::GetInstance()->Render();
}

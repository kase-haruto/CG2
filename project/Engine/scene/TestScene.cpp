/////////////////////////////////////////////////////////////////////////////////////////
//	include
/////////////////////////////////////////////////////////////////////////////////////////

// scene
#include "../scene/TestScene.h"

// engine
#include "../core/Input.h"
#include "../core/Audio/Audio.h"
#include "../graphics/camera/CameraManager.h"
#include "Engine/objects/particle/ParticleManager.h"
#include "Engine/Collision/CollisionManager.h"
#include "Engine/core/DirectX/DxCore.h"
#include "Engine/objects/SceneObjectManager.h"
#include "Engine/core/UI/EditorPanel.h"

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
	teapot_ = std::make_unique<BaseGameObject>("teapot.obj");
	teapot_->SetName("teapot");
	teapot_->SetTranslate({5.0f, 0.0f, 0.0f});

	/////////////////////////////////////////////////////////////////////////////////////////
	//							editor
	/////////////////////////////////////////////////////////////////////////////////////////
	//モデル
	modelBuilder_ = std::make_unique<ModelBuilder>();
	modelBuilder_->Initialize();

	//sprite
	uiEditor_ = std::make_unique<UIEditor>();

	// uiに追加
	EditorPanel* editorPanel = EngineUI::GetInstance()->GetPanel<EditorPanel>();
	editorPanel->AddEditor(modelBuilder_.get());
	editorPanel->AddEditor(uiEditor_.get());

}

/////////////////////////////////////////////////////////////////////////////////////////
//	更新処理
/////////////////////////////////////////////////////////////////////////////////////////
void TestScene::Update(){
#ifdef _DEBUG
#endif //  _DEBUG

	CameraManager::Update();

	//uiの更新
	uiEditor_->Update();

	//モデルの更新
	modelBuilder_->Update();

	//地面
	modelField_->Update();

	//test
	bunny_->Update();
	teapot_->Update();

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
	//モデルの描画
	modelBuilder_->Draw();

	//地面の描画
	modelField_->Draw();

	//test
	bunny_->Draw();
	teapot_->Draw();

	//particle描画
	ParticleManager::GetInstance()->Draw();

	//primitiveな描画
	PrimitiveDrawer::GetInstance()->Render();


#pragma endregion

	/////////////////////////////////////////////////////////////////////////////////////////
	//					2dオブジェクトの描画
	/////////////////////////////////////////////////////////////////////////////////////////
#pragma region 2Dオブジェクト描画
	uiEditor_->Draw();

#pragma endregion
}

void TestScene::CleanUp(){
	SceneObjectManager::GetInstance()->ClearAllObject();
}

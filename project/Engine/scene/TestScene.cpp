#include "../scene/TestScene.h"

#include "../core/Input.h"
#include "../core/Audio/Audio.h"

#include "../graphics/camera/CameraManager.h"

#include "Engine/objects/particle/ParticleManager.h"

#include "Engine/Collision/CollisionManager.h"

#include "Engine/core/DirectX/DxCore.h"

#include "lib/myFunc/MyFunc.h"

TestScene::TestScene(){}

TestScene::TestScene(DxCore* dxCore) : IScene(dxCore){
	// シーン名を設定
	IScene::SetSceneName("testScene");
}

void TestScene::Initialize(){
	CameraManager::GetInstance()->SetType(CameraType::Type_Debug);
	///=========================
	/// グラフィック関連
	///=========================
	directionalLight_ = std::make_unique<DirectionalLight>();
	directionalLight_->Initialize(pDxCore_);

	pointLight_ = std::make_unique<PointLight>();
	pointLight_->Initialize(pDxCore_);

	fog_ = std::make_unique<FogEffect>(pDxCore_);

	modelField_ = std::make_unique<Model>("ground.obj");
	modelField_->SetSize({100.0f,1.0f,100.0f});
	modelField_->SetUvScale({15.0f,15.0f,0.0f});

	/////////////////////////////////////////////////////////////////////////////////////////
	//							editor
	/////////////////////////////////////////////////////////////////////////////////////////
	//モデル
	modelBuilder_ = std::make_unique<ModelBuilder>();
	modelBuilder_->Initialize();

	animationCube_ = std::make_unique<AnimationModel>("AnimatedCube.gltf");

	spriteTestModel_ = std::make_unique<Model>("plane.obj");
	spriteTestModel_->InitializeTextures({"thunder1.png","thunder2.png",
										 "thunder3.png","thunder4.png",
										 "thunder5.png","thunder6.png",
										 "thunder7.png"});

	spriteTestModel_->SetPos({10.0f,1.0f,0.0f});


	//sprite
	uiEditor_ = std::make_unique<UIEditor>();


}

void TestScene::Update(){
	CameraManager::Update();


#ifdef _DEBUG

#endif // _DEBUG

	//uiの更新
	uiEditor_->Update();
	spriteTestModel_->Update();

	//モデルの更新
	modelBuilder_->Update();

	modelField_->Update();

	animationCube_->AnimationUpdate();


	CollisionManager::GetInstance()->UpdateCollisionAllCollider();

}

void TestScene::Draw(){
	/////////////////////////////////////////////////////////////////////////////////////////
	//					3dオブジェクトの描画
	/////////////////////////////////////////////////////////////////////////////////////////
#pragma region 3Dオブジェクト描画
	//3dオブジェクト描画前処理
	ModelPreDraw();

	//モデルの描画
	modelBuilder_->Draw();

	//地面の描画
	modelField_->Draw();

	animationCube_->Draw();

	spriteTestModel_->Draw();

	ParticleManager::GetInstance()->Draw();

	PrimitiveDrawer::GetInstance()->Render();


#pragma endregion


	/////////////////////////////////////////////////////////////////////////////////////////
	//					2dオブジェクトの描画
	/////////////////////////////////////////////////////////////////////////////////////////
#pragma region 2Dオブジェクト描画


#pragma endregion
}

void TestScene::CleanUp(){
}

void TestScene::ModelPreDraw(){
	pointLight_->Render();
	directionalLight_->Render();
	fog_->Update();
}

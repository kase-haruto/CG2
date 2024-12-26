#include "../scene/TestScene.h"

#include "../core/Input.h"

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
	CameraManager::Initialize();
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

	//sprite
	uiEditor_ = std::make_unique<UIEditor>();

}

void TestScene::Update(){
	CameraManager::Update();

#ifdef _DEBUG
	ImGui::Begin("debugWindow");
	
	Quaternion rotate0 = Quaternion::MakeRotateAxisQuaternion({0.71f,0.71f,0.0f}, 0.3f);
	Quaternion rotate1 = {-rotate0.x,-rotate0.y,-rotate0.z,-rotate0.w};

	Quaternion interpolate0 = Quaternion::Slerp(rotate0, rotate1, 0.0f);
	Quaternion interpolate1 = Quaternion::Slerp(rotate0, rotate1, 0.3f);
	Quaternion interpolate2 = Quaternion::Slerp(rotate0, rotate1, 0.5f);
	Quaternion interpolate3 = Quaternion::Slerp(rotate0, rotate1, 0.7f);
	Quaternion interpolate4 = Quaternion::Slerp(rotate0, rotate1, 1.0f);

	// オリジナルのクォータニオンを表示
	ImGui::Text("Rotate0: [%.2f, %.2f, %.2f, %.2f]", rotate0.x, rotate0.y, rotate0.z, rotate0.w);
	ImGui::Text("Rotate1: [%.2f, %.2f, %.2f, %.2f]", rotate1.x, rotate1.y, rotate1.z, rotate1.w);

	// 補間結果を表示
	ImGui::Text("Interpolate (t=0.0): [%.2f, %.2f, %.2f, %.2f]", interpolate0.x, interpolate0.y, interpolate0.z, interpolate0.w);
	ImGui::Text("Interpolate (t=0.3): [%.2f, %.2f, %.2f, %.2f]", interpolate1.x, interpolate1.y, interpolate1.z, interpolate1.w);
	ImGui::Text("Interpolate (t=0.5): [%.2f, %.2f, %.2f, %.2f]", interpolate2.x, interpolate2.y, interpolate2.z, interpolate2.w);
	ImGui::Text("Interpolate (t=0.7): [%.2f, %.2f, %.2f, %.2f]", interpolate3.x, interpolate3.y, interpolate3.z, interpolate3.w);
	ImGui::Text("Interpolate (t=1.0): [%.2f, %.2f, %.2f, %.2f]", interpolate4.x, interpolate4.y, interpolate4.z, interpolate4.w);

	ImGui::End();
#endif // _DEBUG


	//uiの更新
	uiEditor_->Update();

	//モデルの更新
	modelBuilder_->Update();

	modelField_->Update();

	animationCube_->Update();


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

	ParticleManager::GetInstance()->Draw();

	PrimitiveDrawer::GetInstance()->Render();


#pragma endregion


	/////////////////////////////////////////////////////////////////////////////////////////
	//					2dオブジェクトの描画
	/////////////////////////////////////////////////////////////////////////////////////////
#pragma region 2Dオブジェクト描画
	//uiの描画
	uiEditor_->Draw();

#pragma endregion
}

void TestScene::CleanUp(){
}

void TestScene::ModelPreDraw(){
	pointLight_->Render();
	directionalLight_->Render();
	fog_->Update();
}

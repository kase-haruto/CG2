#include "TestScene.h"

TestScene::TestScene(){}

TestScene::~TestScene(){}

void TestScene::Initialize(){
	///=========================
	/// グラフィック関連
	///=========================
	dxCommon_ = DirectXCommon::GetInstance();

	///=========================
	/// カメラ関連
	///=========================
	viewProjection_ = std::make_unique<ViewProjection>(dxCommon_);

	///=========================
	/// オブジェクト関連
	///=========================
    //スプライト 
	sprite_ = std::make_unique<Sprite>(dxCommon_);
	sprite_->Initialize();

	

	//モデル
	model_ = std::make_unique<Model>();
	model_->Initialize(dxCommon_,viewProjection_.get());

	//球体
	sphere_ = std::make_unique<Sphere>();
	sphere_->Initialize(dxCommon_,viewProjection_.get());

	//三角
	triangle_ = std::make_unique<Triangle>();
	triangle_->Initialize(dxCommon_,viewProjection_.get());
}

void TestScene::Update(){
	//モデルの更新
	model_->Update();
	//球体の更新
	sphere_->Update();
	//三角形の更新
	triangle_->UpdateImGui("triangle");
	triangle_->Update();
}

void TestScene::Draw(){
	//モデルの描画
	model_->Draw();
	////球体の描画
	//sphere_->Draw();
	////三角形の描画
	//triangle_->Draw();
}

void TestScene::Finalize(){

}
#include "TestScene.h"

TestScene::TestScene(){}

TestScene::~TestScene(){}

void TestScene::Initialize(){
	///=========================
	/// グラフィック関連
	///=========================


	///=========================
	/// カメラ関連
	///=========================
	viewProjection_ = std::make_unique<ViewProjection>();

	///=========================
	/// オブジェクト関連
	///=========================
    //スプライト 
	sprite_ = std::make_unique<Sprite>();
	sprite_->Initialize();

	//モデル
	modelBuilder_ = std::make_unique<ModelBuilder>();
	modelBuilder_->SetViewProjection(viewProjection_.get());


	sphere_ = std::make_unique<Sphere>();
	sphere_->Initialize(viewProjection_.get());
}

void TestScene::Update(){
	modelBuilder_->ShowImGuiInterface();
	sphere_->UpdateImGui("sphere");

	//モデルの更新
	modelBuilder_->Update();

	sphere_->Update();

	sprite_->Update();
}

void TestScene::Draw(){
	//モデルの描画
	modelBuilder_->Draw();

	sphere_->Draw();

	sprite_->Draw();
}

void TestScene::Finalize(){
	viewProjection_.reset();
	modelBuilder_.reset();
	sprite_.reset();
	sphere_.reset();
}
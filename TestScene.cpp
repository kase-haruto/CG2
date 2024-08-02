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
}

void TestScene::Update(){
	modelBuilder_->ShowImGuiInterface();

	//モデルの更新
	modelBuilder_->Update();
}

void TestScene::Draw(){
	//モデルの描画
	modelBuilder_->Draw();
	//sprite_->Draw();
}

void TestScene::Finalize(){
	viewProjection_.reset();
	model_.reset();
}
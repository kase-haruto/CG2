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
	model_ = std::make_unique<Model>();
	model_->SetViewProjection(viewProjection_.get());
	model_->Create("Resources", "suzanne.obj");

}

void TestScene::Update(){
	//モデルの更新
	model_->Update();
}

void TestScene::Draw(){
	//モデルの描画
	model_->Draw();
	//sprite_->Draw();
}

void TestScene::Finalize(){
	viewProjection_.reset();
	model_.reset();
}
#include "TestScene.h"

TestScene::TestScene(){}

void TestScene::Initialize(){
	///=========================
	/// グラフィック関連
	///=========================


	///=========================
	/// カメラ関連
	///=========================
	viewProjection_ = std::make_unique<ViewProjection>();
	viewProjection_->Initialize();

	///=========================
	/// オブジェクト関連
	///=========================
    //スプライト 
	sprite_ = std::make_unique<Sprite>();
	sprite_->Initialize();

	//モデル
	modelBuilder_ = std::make_unique<ModelBuilder>();
	modelBuilder_->SetViewProjection(viewProjection_.get());

	////パーティクル
	//particle_ = std::make_unique<ParticleManager>(50);//パーティクルの最大数を10個に設定
	//particle_->Create(viewProjection_.get());

	//球体
	sphere_ = std::make_unique<Sphere>();
	sphere_->Initialize(viewProjection_.get());

	////地面
	modelGround_ = std::make_unique<Model>();
	modelGround_->Create("ground");
	modelGround_->SetViewProjection(viewProjection_.get());
	modelGround_->SetSize({100.0f,0.0f,100.0f});
	modelGround_->SetUvScale({30.0f,30.0f,0.0f});

	//modelField_ = std::make_unique<Model>();
	//modelField_->Create("Resources", "terrain.obj");
	//modelField_->SetViewProjection(viewProjection_.get());

}

void TestScene::Update(){
#ifdef _DEBUG
	modelBuilder_->ShowImGuiInterface();
	sphere_->UpdateImGui("sphere");
	viewProjection_->ImGui();
	modelGround_->ShowImGuiInterface();
	//modelField_->ShowImGuiInterface();
#endif // _DEBUG

	

	//モデルの更新
	modelBuilder_->Update();

	/*modelField_->Update();*/

	modelGround_->Update();

	//particle_->Update();

	sphere_->Update();


	sprite_->Update();
}

void TestScene::Draw(){
	//モデルの描画
	modelBuilder_->Draw();

	//particle_->Draw();

	sphere_->Draw();

	modelGround_->Draw();

	/*modelField_->Draw();*/

	sprite_->Draw();
}

void TestScene::Finalize(){
	viewProjection_.reset();
	modelBuilder_.reset();
	sprite_.reset();
	sphere_.reset();
	modelGround_.reset();
	/*modelField_.reset();*/
	//particle_.reset();
}
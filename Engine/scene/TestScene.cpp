#include "TestScene.h"

#include "Input.h"
#include "myFunc/PrimitiveDrawer.h"

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
	//線
	PrimitiveDrawer::GetInstance()->SetViewProjection(viewProjection_.get());
	PrimitiveDrawer::GetInstance()->Initialize();
	
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
	/*modelGround_ = std::make_unique<Model>("ground");
	modelGround_->SetViewProjection(viewProjection_.get());
	modelGround_->SetSize({100.0f,0.0f,100.0f});
	modelGround_->SetUvScale({30.0f,30.0f,0.0f});*/

	modelField_ = std::make_unique<Model>();
	modelField_->Create("terrain");
	modelField_->SetViewProjection(viewProjection_.get());
}

void TestScene::Update(){
#ifdef _DEBUG
	modelBuilder_->ShowImGuiInterface();
	sphere_->UpdateImGui("sphere");
	viewProjection_->ImGui();
	//modelGround_->ShowImGuiInterface();
	//modelField_->ShowImGuiInterface();
#endif // _DEBUG

	/////////////////////////////////////////////////////////////////////////////////////////
	//		カメラの更新
	/////////////////////////////////////////////////////////////////////////////////////////

	if (Input::PushKey(DIK_A)){
		viewProjection_->transform.translate.x -= 0.1f;
	} else if (Input::PushKey(DIK_D)){
		viewProjection_->transform.translate.x += 0.1f;
	}

	if (Input::PushKey(DIK_W)){
		viewProjection_->transform.translate.z += 0.1f;
	} else if (Input::PushKey(DIK_S)){
		viewProjection_->transform.translate.z -= 0.1f;
	}

	if (Input::PushKey(DIK_RIGHT)){
		viewProjection_->transform.rotate.y += 0.05f;
	} else if (Input::PushKey(DIK_LEFT)){
		viewProjection_->transform.rotate.y -= 0.05f;
	}


	//モデルの更新
	modelBuilder_->Update();

	modelField_->Update();

	/*modelGround_->Update();*/

	//particle_->Update();

	sphere_->Update();


	sprite_->Update();
}

void TestScene::Draw(){
	//モデルの描画
	modelBuilder_->Draw();

	//particle_->Draw();

	sphere_->Draw();

	/*modelGround_->Draw();*/

	modelField_->Draw();

	sprite_->Draw();


	PrimitiveDrawer::GetInstance()->DrawLine3d(Vector3 {0.0f,5.0f,0.0f}, Vector3 {2.0f,1.0f,0.0f}, Vector4 {1.0f,0.0f,0.0f,1.0f});

	PrimitiveDrawer::GetInstance()->Render();

}

void TestScene::Finalize(){
	viewProjection_.reset();
	modelBuilder_.reset();
	sprite_.reset();
	sphere_.reset();
	/*modelGround_.reset();*/
	PrimitiveDrawer::GetInstance()->Finalize();
	modelField_.reset();
	//particle_.reset();
}
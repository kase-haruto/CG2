#include "GameScene.h"
#include"GlobalVariable/GlobalVariables.h"
#include "myFunc/PrimitiveDrawer.h"
#include "core/Input.h"

GameScene::GameScene(){
	GlobalVariables::GetInstance()->Initialize();
}

void GameScene::Initialize(){

	///=========================
	/// カメラ関連
	///=========================
	viewProjection_ = std::make_unique<ViewProjection>();
	viewProjection_->Initialize();

	///=========================
	/// 線の描画
	///=========================
	PrimitiveDrawer::GetInstance()->SetViewProjection(viewProjection_.get());
	PrimitiveDrawer::GetInstance()->Initialize();

	///=========================
	/// Editor関連
	///=========================
	railEditor_ = std::make_unique<RailEditor>();
	railEditor_->SetViewProjection(viewProjection_.get());
	railEditor_->Initialize();

	

	/////////////////////////////////////////////////////////////////////////////////////////
	//				レール
	/////////////////////////////////////////////////////////////////////////////////////////
	rail_.SetCtrlPoints(railEditor_->GetControlPoint());

	railCamera_ = std::make_unique<RailCamera>();
	railCamera_->SetCtrlPoints(railEditor_->GetControlPoint());

	/////////////////////////////////////////////////////////////////////////////////////////
	//				プレイヤー
	/////////////////////////////////////////////////////////////////////////////////////////
	playerModel_ = std::make_unique<Model>("debugCube");

	player_ = std::make_unique<Player>();
	player_->Initialize(playerModel_.get());
	player_->SetViewProjection(viewProjection_.get());
	player_->SetCtrlPoints(railEditor_->GetControlPoint());

	player_->SetParent(&railCamera_->GetTransform());
}

void GameScene::Update(){

#ifdef _DEBUG
	viewProjection_->ImGui();
	GlobalVariables::GetInstance()->Update();

	railEditor_->Update();
	rail_.SetCtrlPoints(railEditor_->GetControlPoint());

	ImGui::Begin("rail");

	// ボタンが押されたらフラグを反転させる
	if (ImGui::Button("camera")){
		isRail_ = !isRail_;
	}

	ImGui::End();
#endif // _DEBUG
		viewProjection_->transform.translate = player_->GetTransform().translate;
		viewProjection_->transform.rotate = player_->GetTransform().rotate;

	

	/////////////////////////////////////////////////////////////////////////////////////////
	//		カメラの更新
	/////////////////////////////////////////////////////////////////////////////////////////

	if (Input::PushKey(DIK_A)){
		viewProjection_->transform.translate.x -= 0.1f;
	} else if (Input::PushKey(DIK_D)){
		viewProjection_->transform.translate.x += 0.1f;
	}

	if (Input::PushKey(DIK_SPACE)){
		viewProjection_->transform.translate.y += 0.1f;
	} else if (Input::PushKey(DIK_LSHIFT)){
		viewProjection_->transform.translate.y -= 0.1f;
	}

	if (Input::PushKey(DIK_W)){
		viewProjection_->transform.translate.z += 0.1f;
	} else if (Input::PushKey(DIK_S)){
		viewProjection_->transform.translate.z -= 0.1f;
	}

	if (Input::PushKey(DIK_L)){
		viewProjection_->transform.rotate.y += 0.02f;
	} else if (Input::PushKey(DIK_J)){
		viewProjection_->transform.rotate.y -= 0.02f;
	}



	railCamera_->Update();

	//playerの更新
	player_->Update();
}

void GameScene::Draw(){
	railEditor_->Draw();

	//playerの描画
	player_->Draw();

	PrimitiveDrawer::GetInstance()->Render();
}

void GameScene::Finalize(){
	viewProjection_.reset();
	railEditor_.reset();
	player_.reset();
	playerModel_.reset();

	PrimitiveDrawer::GetInstance()->Finalize();
}
#include "GameScene.h"
#include"GlobalVariable/GlobalVariables.h"
#include "myFunc/PrimitiveDrawer.h"
#include "core/Input.h"

GameScene::GameScene(){
	GlobalVariables::GetInstance()->Initialize();
}

static Vector3 originPos;
static Vector3 originRotate;

void GameScene::Initialize(){

	///=========================
	/// カメラ関連
	///=========================
	viewProjection_ = std::make_unique<ViewProjection>();
	viewProjection_->Initialize();
	originPos = viewProjection_->transform.translate;
	originRotate = viewProjection_->transform.rotate;

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
	playerModel_ = std::make_unique<Model>("suzanne");

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
		player_->SetIsRail(isRail_);
	}

	ImGui::End();
#endif // _DEBUG
	if (isRail_){
		viewProjection_->transform.translate = railCamera_->GetTransform().translate;
		viewProjection_->transform.rotate = railCamera_->GetTransform().rotate;
	} else{
		viewProjection_->transform.translate = originPos;
		viewProjection_->transform.rotate = originRotate;

	}
		

	/////////////////////////////////////////////////////////////////////////////////////////
	//		カメラの更新
	/////////////////////////////////////////////////////////////////////////////////////////

	if (Input::PushKey(DIK_A)){
		originPos.x -= 0.1f;
	} else if (Input::PushKey(DIK_D)){
		originPos.x += 0.1f;
	}

	if (Input::PushKey(DIK_SPACE)){
		originPos.y += 0.1f;
	} else if (Input::PushKey(DIK_LSHIFT)){
		originPos.y -= 0.1f;
	}

	if (Input::PushKey(DIK_W)){
		originPos.z += 0.1f;
	} else if (Input::PushKey(DIK_S)){
		originPos.z -= 0.1f;
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
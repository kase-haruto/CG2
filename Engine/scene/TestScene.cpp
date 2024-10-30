#include "TestScene.h"

#include "Input.h"

TestScene::TestScene(){}

TestScene::TestScene(DxCore* dxCore) : IScene(dxCore){}

void TestScene::Initialize(){
	///=========================
	/// グラフィック関連
	///=========================
	directionalLight_ = std::make_unique<DirectionalLight>();
	directionalLight_->Initialize(pDxCore_);

	pointLight_ = std::make_unique<PointLight>();
	pointLight_->Initialize(pDxCore_);

	fog_ = std::make_unique<FogEffect>(pDxCore_);

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

	//パーティクル
	particle_ = std::make_unique<ParticleManager>(50);//パーティクルの最大数を10個に設定
	particle_->Create(viewProjection_.get());

	//球体
	sphere_ = std::make_unique<Sphere>();
	sphere_->Initialize(viewProjection_.get());

	////地面
	/*modelGround_ = std::make_unique<Model>("ground");
	modelGround_->SetViewProjection(viewProjection_.get());
	modelGround_->SetSize({100.0f,0.0f,100.0f});
	modelGround_->SetUvScale({30.0f,30.0f,0.0f});*/

	modelField_ = std::make_unique<Model>("terrain");
	modelField_->SetViewProjection(viewProjection_.get());


	/////////////////////////////////////////////////////////////////////////////////////////
	//							editor
	/////////////////////////////////////////////////////////////////////////////////////////
	//モデル
	modelBuilder_ = std::make_unique<ModelBuilder>();
	modelBuilder_->SetViewProjection(viewProjection_.get());

	//sprite
	uiEditor_ = std::make_unique<UIEditor>();
}

void TestScene::Update(){
#ifdef _DEBUG
	UpdateDebugUI();
#endif // _DEBUG

	//uiの更新
	uiEditor_->Update();

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

	particle_->Update();

	sphere_->Update();

}

void TestScene::UpdateDebugUI(){
	static int selectedObjectIndex = -1;
	static int selectedEditorIndex = -1;

	// オブジェクト名のリスト
	std::vector<const char*> objectNames = {"sphere","directionalLight","pointLight"};

	// 複数のエディタのリスト（BaseEditor*で管理）
	std::vector<BaseEditor*> editors = {modelBuilder_.get(),uiEditor_.get()};

	// メインウィンドウ
	ImGui::Begin("Main GUI");

	// Objectセクション
	if (ImGui::CollapsingHeader("Object Settings")){
		ImGui::Text("Select an object to edit its properties:");

		// オブジェクトのリスト表示（リストボックス形式）
		if (ImGui::ListBox("Objects", &selectedObjectIndex, objectNames.data(), static_cast< int >(objectNames.size()))){
			// オブジェクト選択時の処理はここで必要なら行う
		}

		// 選択されたオブジェクトの設定表示
		if (selectedObjectIndex >= 0 && selectedObjectIndex < static_cast< int >(objectNames.size())){
			ImGui::Separator();
			ImGui::Text("Editing: %s", objectNames[selectedObjectIndex]);

			// 各オブジェクトの設定表示
			if (selectedObjectIndex == 0 && sphere_){
				sphere_->UpdateImGui("sphere");
			}
			//directionalLight
			else if (selectedObjectIndex == 1 && directionalLight_){
				directionalLight_->ShowImGuiInterFace();
			}
			//pointLight
			else if (selectedObjectIndex == 2 && pointLight_){
				pointLight_->ShowImGuiInterface();
			}

			//fog
			else if (selectedObjectIndex == 3 && fog_){
				fog_->ShowImGuiInterface();
			}
		}
	}

	// Editorセクション
	if (ImGui::CollapsingHeader("Editor Settings")){
		ImGui::Text("Select an editor to configure:");

		// エディタのリスト表示（リストボックス形式）
		if (ImGui::ListBox("Editors", &selectedEditorIndex, [] (void* data, int idx, const char** out_text){
			auto* editors = reinterpret_cast< std::vector<BaseEditor*>* >(data);
			*out_text = (*editors)[idx]->GetEditorName(); // エディタ名を取得
			return true;
			}, &editors, static_cast< int >(editors.size()))){ // size_t から int へキャスト
			// エディタ選択時の処理はここで必要なら行う
		}

		// 選択されたエディタの設定表示
		if (selectedEditorIndex >= 0 && selectedEditorIndex < static_cast< int >(editors.size())){
			ImGui::Separator();
			ImGui::Text("Editing Editor %d", selectedEditorIndex + 1);

			// 選択されたエディタのUIを表示
			editors[selectedEditorIndex]->ShowImGuiInterface();
		}

	}

	ImGui::End();

	viewProjection_->ImGui();
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

	//球体の描画
	sphere_->Draw();

	/*modelGround_->Draw();*/

	//地面の描画
	modelField_->Draw();

#pragma endregion


	/////////////////////////////////////////////////////////////////////////////////////////
	//					2dオブジェクトの描画
	/////////////////////////////////////////////////////////////////////////////////////////
#pragma region 2Dオブジェクト描画
	//uiの描画
	uiEditor_->Draw();

	particle_->Draw();

	PrimitiveDrawer::GetInstance()->Render();
#pragma endregion
}

void TestScene::Finalize(){
	viewProjection_.reset();
	modelBuilder_.reset();
	uiEditor_.reset();
	sphere_.reset();
	/*modelGround_.reset();*/
	PrimitiveDrawer::GetInstance()->Finalize();
	modelField_.reset();
	particle_.reset();
}

void TestScene::ModelPreDraw(){
	pointLight_->Render();
	directionalLight_->Render();
	fog_->Update();
	viewProjection_->UpdateMatrix();
}

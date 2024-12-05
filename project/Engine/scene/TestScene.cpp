#include "../scene/TestScene.h"

#include "../core/Input.h"

#include "../graphics/camera/CameraManager.h"

#include "Engine/objects/particle/ParticleManager.h"

#include "Engine/Collision/CollisionManager.h"

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
	CameraManager::Initialize();

	///=========================
	/// オブジェクト関連
	///=========================
	//線
	PrimitiveDrawer::GetInstance()->Initialize();

	////地面
	/*modelGround_ = std::make_unique<Model>("ground");
	modelGround_->SetViewProjection(viewProjection_.get());
	modelGround_->SetSize({100.0f,0.0f,100.0f});
	modelGround_->SetUvScale({30.0f,30.0f,0.0f});*/

	modelField_ = std::make_unique<Model>("terrain");

	demoObject_ = std::make_unique<GameObject_Demo>("debugCube");
	demoObject_->Initialize();

	testObject_ = std::make_unique<TestObject>("debugCube");
	testObject_->Initialize();

	/////////////////////////////////////////////////////////////////////////////////////////
	//							editor
	/////////////////////////////////////////////////////////////////////////////////////////
	//モデル
	modelBuilder_ = std::make_unique<ModelBuilder>();
	modelBuilder_->Initialize();

	//sprite
	uiEditor_ = std::make_unique<UIEditor>();
}

void TestScene::Update(){
#ifdef _DEBUG
	UpdateDebugUI();
#endif // _DEBUG

	CameraManager::Update();

	//uiの更新
	uiEditor_->Update();

	//モデルの更新
	modelBuilder_->Update();

	modelField_->Update();

	demoObject_->Update();
	testObject_->Update();

	CollisionManager::GetInstance()->UpdateCollisionAllCollider();

	/*modelGround_->Update();*/

}

void TestScene::UpdateDebugUI(){
#ifdef _DEBUG
	static int selectedObjectIndex = -1;
	static int selectedEditorIndex = -1;

	CollisionManager::GetInstance()->DebugLog();

	// オブジェクト名のリスト
	std::vector<const char*> objectNames = {"directionalLight", "pointLight"};

	// 複数のエディタのリスト（BaseEditor*で管理）
	std::vector<BaseEditor*> editors = {modelBuilder_.get(), uiEditor_.get()};

	// メインウィンドウ
	ImGui::Begin("Main GUI");
	if (ImGui::BeginTabBar("MyTabBar")){

		// Demo Object タブ
		demoObject_->ShowDebugUI();

		// Object Settings タブ
		if (ImGui::BeginTabItem("Object Settings")){
			ImGui::Text("Select an object to edit its properties:");

			// オブジェクトのリスト表示（リストボックス形式）
			if (ImGui::ListBox("Objects", &selectedObjectIndex, objectNames.data(), static_cast< int >(objectNames.size()))){
				// オブジェクト選択時の処理はここで必要なら行う
			}

			// 選択されたオブジェクトの設定表示
			if (selectedObjectIndex >= 0 && selectedObjectIndex < static_cast< int >(objectNames.size())){
				ImGui::Separator();
				ImGui::Text("Editing: %s", objectNames[selectedObjectIndex]);

				// directionalLight
				if (selectedObjectIndex == 0 && directionalLight_){
					directionalLight_->ShowImGuiInterFace();
				}
				// pointLight
				else if (selectedObjectIndex == 1 && pointLight_){
					pointLight_->ShowImGuiInterface();
				}
				// fog
				else if (selectedObjectIndex == 2 && fog_){
					fog_->ShowImGuiInterface();
				}
			}
			ImGui::EndTabItem();
		}

		// Editor Settings タブ
		if (ImGui::BeginTabItem("Editor Settings")){
			ImGui::Text("Select an editor to configure:");

			// エディタのリスト表示（リストボックス形式）
			if (ImGui::ListBox("Editors", &selectedEditorIndex, [] (void* data, int idx, const char** out_text){
				auto* editors = reinterpret_cast< std::vector<BaseEditor*>* >(data);
				*out_text = (*editors)[idx]->GetEditorName(); // エディタ名を取得
				return true;
				}, &editors, static_cast< int >(editors.size()))){
				// エディタ選択時の処理はここで必要なら行う
			}

			// 選択されたエディタの設定表示
			if (selectedEditorIndex >= 0 && selectedEditorIndex < static_cast< int >(editors.size())){
				ImGui::Separator();
				ImGui::Text("Editing Editor %d", selectedEditorIndex + 1);

				// 選択されたエディタのUIを表示
				editors[selectedEditorIndex]->ShowImGuiInterface();
			}
			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}

	ImGui::End();

#endif // _DEBUG
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

	demoObject_->Draw();
	testObject_->Draw();

	/*modelGround_->Draw();*/

	//地面の描画
	modelField_->Draw();

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

void TestScene::Finalize(){
	modelBuilder_.reset();
	uiEditor_.reset();
	/*modelGround_.reset();*/
	PrimitiveDrawer::GetInstance()->Finalize();
	ParticleManager::GetInstance()->Finalize();
	modelField_.reset();
}

void TestScene::ModelPreDraw(){
	pointLight_->Render();
	directionalLight_->Render();
	fog_->Update();
}

#include "../scene/TestScene.h"

#include "../core/Input.h"

#include "../graphics/camera/CameraManager.h"

#include "Engine/objects/particle/ParticleManager.h"

#include "Engine/Collision/CollisionManager.h"

#include "Engine/core/DirectX/DxCore.h"

TestScene::TestScene(){}

TestScene::TestScene(DxCore* dxCore) : IScene(dxCore){
	// シーン名を設定
	IScene::SetSceneName("testScene");
}

void TestScene::Initialize(){
	///=========================
	/// グラフィック関連
	///=========================
	directionalLight_ = std::make_unique<DirectionalLight>();
	directionalLight_->Initialize(pDxCore_);

	pointLight_ = std::make_unique<PointLight>();
	pointLight_->Initialize(pDxCore_);

	fog_ = std::make_unique<FogEffect>(pDxCore_);

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

	demoParticle_ = std::make_unique<DemoParticle>();
	demoParticle_->Initialize("plane", "particle.png");

	tornadoParticle_ = std::make_unique<TornadoParticle>();
	tornadoParticle_->Initialize("debugCube", "white1x1.png");

	/////////////////////////////////////////////////////////////////////////////////////////
	//							editor
	/////////////////////////////////////////////////////////////////////////////////////////
	//モデル
	modelBuilder_ = std::make_unique<ModelBuilder>();
	modelBuilder_->Initialize();

	//sprite
	uiEditor_ = std::make_unique<UIEditor>();

	/////////////////////////////////////////////////////////////////////////////////////////
	//							ui
#ifdef _DEBUG

	pEngineUI_->SetMainViewportCallback([this] (){
		ImVec2 viewportSize = ImGui::GetContentRegionAvail();
		if (pDxCore_->GetRenderTarget().offscreenSrvGpuDescriptorHandle_.ptr != 0){
			ImGui::Image(reinterpret_cast< ImTextureID >(pDxCore_->GetRenderTarget().offscreenSrvGpuDescriptorHandle_.ptr), viewportSize);
		} else{
			ImGui::Text("Viewport not ready");
		}
										});
	//// エンジンUIの初期化

	//// ツールバーの描画処理
	//pEngineUI_->SetToolbarCallback([] (){
	//	ImGui::Button("Edit");
	//	ImGui::SameLine();
	//	ImGui::Button("Play");
	//	ImGui::SameLine();
	//	ImGui::Button("Settings");
	//							});

	//// パネルの追加
	//pEngineUI_->AddPanelCallback("Object Settings", [this] (){
	//	static int selectedObjectIndex = -1;

	//	// オブジェクト名のリスト
	//	std::vector<const char*> objectNames = {"directionalLight", "pointLight", "fog"};

	//	// オブジェクトのリストを描画
	//	ImGui::Text("Select an object to edit its properties:");
	//	if (ImGui::ListBox("Objects", &selectedObjectIndex, objectNames.data(), static_cast< int >(objectNames.size()))){
	//		// リストボックスで選択が変更されたときに処理を追加（必要であれば）
	//	}

	//	// 選択されたオブジェクトの設定表示
	//	if (selectedObjectIndex >= 0 && selectedObjectIndex < static_cast< int >(objectNames.size())){
	//		ImGui::Separator();
	//		ImGui::Text("Editing: %s", objectNames[selectedObjectIndex]);

	//		// 選択したオブジェクトに応じたインターフェースを表示
	//		if (selectedObjectIndex == 0 && directionalLight_){
	//			directionalLight_->ShowImGuiInterFace();
	//		} else if (selectedObjectIndex == 1 && pointLight_){
	//			pointLight_->ShowImGuiInterface();
	//		} else if (selectedObjectIndex == 2 && fog_){
	//			fog_->ShowImGuiInterface();
	//		}
	//	}
	//						  });

	//pEngineUI_->AddPanelCallback("gameObject", [this] (){
	//	demoObject_->ShowDebugUI();
	//							 });

	//pEngineUI_->AddPanelCallback("Particles", [] (){
	//	ParticleManager::GetInstance()->ShowDebugUI();
	//						  });

	//pEngineUI_->AddPanelCallback("CollisionLog", [] (){
	//	CollisionManager::GetInstance()->DebugLog();
	//							 });
#endif // _DEBUG

	//// フローティングウィンドウの描画処理
	//pEngineUI_->SetFloatingWindowCallback([] (){
	//	ImGui::Text("Engine Settings");
	//	ImGui::Checkbox("Enable Shadows", nullptr);
	//								   });

}

void TestScene::Update(){
	CameraManager::Update();

	//uiの更新
	uiEditor_->Update();

	//モデルの更新
	modelBuilder_->Update();
	demoParticle_->Update();
	tornadoParticle_->Update();

	modelField_->Update();

	demoObject_->Update();
	testObject_->Update();

	CollisionManager::GetInstance()->UpdateCollisionAllCollider();

	/*modelGround_->Update();*/

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

////////////////////////////////////////////////////////////////////////////////////////////
//  include
////////////////////////////////////////////////////////////////////////////////////////////
#include <Engine/Application/UI/EngineUI/Core/EngineUICore.h>
#include <Engine/Application/System/Enviroment.h>
#include <Engine/Graphics/Camera/Manager/CameraManager.h>

// uiPanel
#include <Engine/Application/UI/Panels/HierarchyPanel.h>
#include <Engine/Application/UI/Panels/EditorPanel.h>
#include <Engine/Application/UI/Panels/InspectorPanel.h>
#include <Engine/Application/UI/Panels/ConsolePanel.h>

// lib
#include <externals/imgui/imgui.h>
#include <externals/imgui/ImGuizmo.h>

////////////////////////////////////////////////////////////////////////////////////////////
//						初期化
////////////////////////////////////////////////////////////////////////////////////////////
void EngineUICore::Initialize() {
	panelController_ = std::make_unique<PanelController>();
	panelController_->Initialize();

	levelEditor_ = std::make_unique<LevelEditor>();
	levelEditor_->Initialize();
}

////////////////////////////////////////////////////////////////////////////////////////////
//						更新
////////////////////////////////////////////////////////////////////////////////////////////
void EngineUICore::Update() {
	levelEditor_->Update();
}

////////////////////////////////////////////////////////////////////////////////////////////
//						レンダリング
////////////////////////////////////////////////////////////////////////////////////////////
void EngineUICore::Render() {
#ifdef _DEBUG
	RenderMenue();
	ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);

	levelEditor_->RenderViewport(ViewportType::VIEWPORT_MAIN,reinterpret_cast< ImTextureID >(mainViewportTextureID_));
	levelEditor_->RenderViewport(ViewportType::VIEWPORT_DEBUG, reinterpret_cast< ImTextureID >(debugViewportTextureID_));

	if (levelEditor_){
		levelEditor_->Render();
	}

	// すべてのパネルをレンダリング
	panelController_->RenderPanels();


#endif // _DEBUG
}

HierarchyPanel* EngineUICore::GetHierarchyPanel() const{
	return levelEditor_->GetHierarchyPanel();
}

EditorPanel* EngineUICore::GetEditorPanel() const{
	return levelEditor_->GetEditorPanel();
}

////////////////////////////////////////////////////////////////////////////////////////////
//						メニューの描画
////////////////////////////////////////////////////////////////////////////////////////////
void EngineUICore::RenderMenue() {
	// メニューバーを開始
	if (ImGui::BeginMainMenuBar()) {
		if (ImGui::BeginMenu("file(F)")) {
			ImGui::MenuItem("新規作成", "Ctrl+N");
			ImGui::MenuItem("開く", "Ctrl+O");
			ImGui::MenuItem("保存", "Ctrl+S");
			ImGui::MenuItem("終了", "Alt+F4");
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("edit(E)")) {
			ImGui::MenuItem("元に戻す", "Ctrl+Z");
			ImGui::MenuItem("やり直し", "Ctrl+Y");
			ImGui::MenuItem("切り取り", "Ctrl+X");
			ImGui::MenuItem("コピー", "Ctrl+C");
			ImGui::MenuItem("貼り付け", "Ctrl+V");
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("view(V)")) {
			ImGui::MenuItem("ツールバーの表示");
			ImGui::MenuItem("ステータスバーの表示");
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("tools(T)")) {
			ImGui::EndMenu();
		}
		// メニューバーを終了
		ImGui::EndMainMenuBar();
	}
}

////////////////////////////////////////////////////////////////////////////////////////////
//						パネル追加
////////////////////////////////////////////////////////////////////////////////////////////
void EngineUICore::AddPanel(std::unique_ptr<IEngineUI> panel) {
	const std::string& name = panel->GetPanelName(); // 先に名前を取り出す
	panelController_->RegisterPanel(name, std::move(panel));
}

////////////////////////////////////////////////////////////////////////////////////////////
//						メインビューポート用のテクスチャを設定
////////////////////////////////////////////////////////////////////////////////////////////
void EngineUICore::SetMainViewportTexture(UINT64 textureID) {
	if (mainViewportTextureID_) { return; }
	mainViewportTextureID_ = textureID;
}

void EngineUICore::SetDebugViewportTexture(UINT64 textureID) {
	if (debugViewportTextureID_) {
		return;
	}
	debugViewportTextureID_ = textureID;
}

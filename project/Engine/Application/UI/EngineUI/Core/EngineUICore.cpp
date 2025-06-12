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

PlaceToolPanel* EngineUICore::GetPlaceToolPanel() const {
	return levelEditor_->GetPlaceToolPanel();
}

////////////////////////////////////////////////////////////////////////////////////////////
//						パネル追加
////////////////////////////////////////////////////////////////////////////////////////////
void EngineUICore::AddPanel(std::unique_ptr<IEngineUI> panel) {
	const std::string& name = panel->GetPanelName(); // 先に名前を取り出す
	panelController_->RegisterPanel(name, std::move(panel));
}

////////////////////////////////////////////////////////////////////////////////////////////
//						sceneの更新を通知
////////////////////////////////////////////////////////////////////////////////////////////
void EngineUICore::NotifySceneContextChanged(SceneContext* newContext) {
	levelEditor_->NotifySceneContextChanged(newContext);
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

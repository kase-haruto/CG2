////////////////////////////////////////////////////////////////////////////////////////////
//  include
////////////////////////////////////////////////////////////////////////////////////////////
#include "EngineUI.h"

// uiPanel
#include "UI/HierarchyPanel.h"
#include "UI/EditorPanel.h"
#include "UI/InspectorPanel.h"
#include "UI/ConsolePanel.h"

// lib
#include <externals/imgui/imgui.h>
#include <externals/imgui/ImGuizmo.h>

////////////////////////////////////////////////////////////////////////////////////////////
//  public変数の初期化
////////////////////////////////////////////////////////////////////////////////////////////
EngineUI* EngineUI::pInstance_ = nullptr;

////////////////////////////////////////////////////////////////////////////////////////////
//  インスタンスの取得
////////////////////////////////////////////////////////////////////////////////////////////
EngineUI* EngineUI::GetInstance(){
	if (!pInstance_){
		pInstance_ = new EngineUI();
	}
	return pInstance_;
}

////////////////////////////////////////////////////////////////////////////////////////////
//                   初期化
////////////////////////////////////////////////////////////////////////////////////////////
void EngineUI::Initialize(){
	GetInstance();

	// 既存のパネルを追加
	pInstance_->panels_.emplace_back(std::make_unique<HierarchyPanel>());
	pInstance_->panels_.emplace_back(std::make_unique<EditorPanel>());
	pInstance_->panels_.emplace_back(std::make_unique<InspectorPanel>());
	pInstance_->panels_.emplace_back(std::make_unique<ConsolePanel>());
}

////////////////////////////////////////////////////////////////////////////////////////////
//                   レンダリング
////////////////////////////////////////////////////////////////////////////////////////////
void EngineUI::Render(){
#ifdef _DEBUG
	pInstance_->RenderMenue();
	ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);
	// すべてのパネルをレンダリングし、閉じられたパネルを削除
	for (auto it = pInstance_->panels_.begin(); it != pInstance_->panels_.end(); ){
		(*it)->Render();
		if (!(*it)->IsShow()){
			// パネルを削除
			it = pInstance_->panels_.erase(it);
		} else{
			++it;
		}
	}

	pInstance_->RenderMainViewport();
#endif // _DEBUG


}

////////////////////////////////////////////////////////////////////////////////////////////
//                   メインビューポートの描画
////////////////////////////////////////////////////////////////////////////////////////////
void EngineUI::RenderMainViewport(){

	ImGui::Begin("Main Viewport");

	if (mainViewportTextureID_){
		// 1. ウィンドウ内で利用可能な領域サイズを取得し、テクスチャ描画
		ImVec2 viewportSize = ImGui::GetContentRegionAvail();
		ImGui::Image(reinterpret_cast< ImTextureID >(mainViewportTextureID_), viewportSize);

		// 2. 直前に描画した Image の矩形領域（スクリーン座標）を取得
		ImVec2 imagePos = ImGui::GetItemRectMin();
		ImVec2 imageSize = ImGui::GetItemRectSize();

		// 3. ギズモの描画先 (DrawList) と、当たり判定を行う矩形を設定
		ImGuizmo::SetDrawlist(ImGui::GetWindowDrawList());
		ImGuizmo::SetRect(imagePos.x, imagePos.y, imageSize.x, imageSize.y);

		// （透視投影なら）
		ImGuizmo::SetOrthographic(false);
	} else{
		ImGui::Text("Viewport texture not set.");
	}

	ImGui::End();

}

////////////////////////////////////////////////////////////////////////////////////////////
//                   メニューの描画
////////////////////////////////////////////////////////////////////////////////////////////
void EngineUI::RenderMenue(){
	// メニューバーを開始
	if (ImGui::BeginMainMenuBar()){
		if (ImGui::BeginMenu("file(F)")){
			ImGui::MenuItem("新規作成", "Ctrl+N");
			ImGui::MenuItem("開く", "Ctrl+O");
			ImGui::MenuItem("保存", "Ctrl+S");
			ImGui::MenuItem("終了", "Alt+F4");
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("edit(E)")){
			ImGui::MenuItem("元に戻す", "Ctrl+Z");
			ImGui::MenuItem("やり直し", "Ctrl+Y");
			ImGui::MenuItem("切り取り", "Ctrl+X");
			ImGui::MenuItem("コピー", "Ctrl+C");
			ImGui::MenuItem("貼り付け", "Ctrl+V");
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("view(V)")){
			ImGui::MenuItem("ツールバーの表示");
			ImGui::MenuItem("ステータスバーの表示");
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("build(B)")){
			ImGui::MenuItem("ビルド");
			ImGui::MenuItem("クリーン");
			ImGui::MenuItem("リビルド");
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("debug(D)")){
			ImGui::MenuItem("開始");
			ImGui::MenuItem("停止");
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("tools(T)")){
			ImGui::EndMenu();
		}
		// メニューバーを終了
		ImGui::EndMainMenuBar();
	}
}

////////////////////////////////////////////////////////////////////////////////////////////
//                   終了処理
////////////////////////////////////////////////////////////////////////////////////////////
void EngineUI::Finalize(){
	delete pInstance_;
	pInstance_ = nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////
//                   パネル追加
////////////////////////////////////////////////////////////////////////////////////////////
void EngineUI::AddPanel(std::unique_ptr<IEngineUI> panel){
	panels_.push_back(std::move(panel));
}

////////////////////////////////////////////////////////////////////////////////////////////
//                   パネルの削除
////////////////////////////////////////////////////////////////////////////////////////////
void EngineUI::RemovePanel(const std::string& panelName){
	auto& panels = pInstance_->panels_;
	panels.erase(
		std::remove_if(panels.begin(), panels.end(),
		[&panelName] (const std::unique_ptr<IEngineUI>& panel) -> bool{
			return panel->GetPanelName() == panelName;
		}),
		panels.end()
	);
}

////////////////////////////////////////////////////////////////////////////////////////////
//                   メインビューポート用のテクスチャを設定
////////////////////////////////////////////////////////////////////////////////////////////
void EngineUI::SetMainViewportTexture(UINT64 textureID){
	if (pInstance_->mainViewportTextureID_){ return; }
	pInstance_->mainViewportTextureID_ = textureID;
}

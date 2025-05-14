////////////////////////////////////////////////////////////////////////////////////////////
//  include
////////////////////////////////////////////////////////////////////////////////////////////
#include <Engine/Application/UI/EngineUI/Renderer/EngineUIRenderer.h>
#include <Engine/Application/System/Enviroment.h>

// uiPanel
#include <Engine/Application/UI/Panels/HierarchyPanel.h>
#include <Engine/Application/UI/Panels/EditorPanel.h>
#include <Engine/Application/UI/Panels/InspectorPanel.h>
#include <Engine/Application/UI/Panels/ConsolePanel.h>

// lib
#include <externals/imgui/imgui.h>
#include <externals/imgui/ImGuizmo.h>

////////////////////////////////////////////////////////////////////////////////////////////
//                   初期化
////////////////////////////////////////////////////////////////////////////////////////////
void EngineUIRenderer::Initialize() {
	panels_.emplace_back(std::make_unique<HierarchyPanel>());
	panels_.emplace_back(std::make_unique<EditorPanel>());
	panels_.emplace_back(std::make_unique<InspectorPanel>());
	panels_.emplace_back(std::make_unique<ConsolePanel>());
}

////////////////////////////////////////////////////////////////////////////////////////////
//                   レンダリング
////////////////////////////////////////////////////////////////////////////////////////////
void EngineUIRenderer::Render() {
#ifdef _DEBUG
	RenderMenue();
	ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);
	RenderMainViewport();
	RenderDebugViewPort();
	// すべてのパネルをレンダリングし、閉じられたパネルを削除
	for (auto it = panels_.begin(); it != panels_.end();) {
		(*it)->Render();
		if (!(*it)->IsShow()) {
			// パネルを削除
			it = panels_.erase(it);
		} else {
			++it;
		}
	}
#endif // _DEBUG
}

////////////////////////////////////////////////////////////////////////////////////////////
//                   メインビューポートの描画
////////////////////////////////////////////////////////////////////////////////////////////
void EngineUIRenderer::RenderMainViewport() {
	ImVec2 viewportSize = ImVec2(kGameViewSize.x, kGameViewSize.y);

	ImGui::SetNextWindowSize(ImVec2(viewportSize.x, viewportSize.y));
	ImGui::Begin("Game View", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar);

	if (mainViewportTextureID_) {
		ImVec2 imagePos = ImGui::GetCursorScreenPos();
		ImGui::SetCursorScreenPos(imagePos);
		ImGui::Image(reinterpret_cast<ImTextureID>(mainViewportTextureID_), viewportSize);
	} else {
		ImGui::Text("Viewport texture not set.");
	}

	ImGui::End();
}

////////////////////////////////////////////////////////////////////////////////////////////
//                   デバッグビューポートの描画
////////////////////////////////////////////////////////////////////////////////////////////
void EngineUIRenderer::RenderDebugViewPort() {
	ImVec2 imgSize{ kExecuteWindowSize.x, kExecuteWindowSize.y };

	ImGui::SetNextWindowSize(imgSize);
	ImGui::Begin("Debug Viewport",
				 nullptr,
				 ImGuiWindowFlags_NoResize |
				 ImGuiWindowFlags_NoScrollbar);

	//--------------------------------------------
	// ① ここで BeginFrame ― ウィンドウが決まった直後
	//--------------------------------------------
	ImGuizmo::BeginFrame();

	if (debugViewportTextureID_) {
		//--------------------------------------------
		// ② 画像を “アイテム無し” で描画
		//--------------------------------------------
		ImVec2 pos = ImGui::GetCursorScreenPos();
		ImDrawList* dl = ImGui::GetWindowDrawList();
		dl->AddImage((ImTextureID)debugViewportTextureID_,
					 pos,
					 ImVec2(pos.x + imgSize.x,
							pos.y + imgSize.y));

		//--------------------------------------------
		// ③ ヒット矩形 & DrawList を登録
		//--------------------------------------------
		ImGuizmo::SetRect(pos.x, pos.y, imgSize.x, imgSize.y);
		ImGuizmo::SetDrawlist();            // ← BeginFrame 後なので有効
	} else {
		ImGui::Text("Viewport texture not set.");
	}

	ImGui::End();
}

////////////////////////////////////////////////////////////////////////////////////////////
//                   メニューの描画
////////////////////////////////////////////////////////////////////////////////////////////
void EngineUIRenderer::RenderMenue() {
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
//                   パネル追加
////////////////////////////////////////////////////////////////////////////////////////////
void EngineUIRenderer::AddPanel(std::unique_ptr<IEngineUI> panel) {
	panels_.push_back(std::move(panel));
}

////////////////////////////////////////////////////////////////////////////////////////////
//                   パネルの削除
////////////////////////////////////////////////////////////////////////////////////////////
void EngineUIRenderer::RemovePanel(const std::string& panelName) {
	panels_.erase(
		std::remove_if(panels_.begin(), panels_.end(),
					   [&panelName](const std::unique_ptr<IEngineUI>& panel) -> bool {
		return panel->GetPanelName() == panelName;
	}),
		panels_.end()
	);
}

////////////////////////////////////////////////////////////////////////////////////////////
//                   メインビューポート用のテクスチャを設定
////////////////////////////////////////////////////////////////////////////////////////////
void EngineUIRenderer::SetMainViewportTexture(UINT64 textureID) {
	if (mainViewportTextureID_) { return; }
	mainViewportTextureID_ = textureID;
}

void EngineUIRenderer::SetDebugViewportTexture(UINT64 textureID) {
	if (debugViewportTextureID_) {
		return;
	}
	debugViewportTextureID_ = textureID;
}

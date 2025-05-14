#include "PanelController.h"
/* ========================================================================
/* include space
/* ===================================================================== */
// uiPanel
#include <Engine/Application/UI/Panels/HierarchyPanel.h>
#include <Engine/Application/UI/Panels/EditorPanel.h>
#include <Engine/Application/UI/Panels/InspectorPanel.h>
#include <Engine/Application/UI/Panels/ConsolePanel.h>

/////////////////////////////////////////////////////////////////////////////////////////
//		panelの取得
/////////////////////////////////////////////////////////////////////////////////////////
IEngineUI* PanelController::GetPanel(const std::string& name) {
	auto it = panels_.find(name);
	if (it != panels_.end()) {
		// パネルが見つかった場合、ポインタを返す
		return it->second.get();
	} else {
		// パネルが見つからない場合、nullptrを返す
		return nullptr;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
//		初期化
/////////////////////////////////////////////////////////////////////////////////////////
void PanelController::Initialize() {
	// 既存のパネルを追加
	panels_.emplace("Hierarchy", std::make_unique<HierarchyPanel>());
	panels_.emplace("Editor", std::make_unique<EditorPanel>());
	panels_.emplace("Inspector", std::make_unique<InspectorPanel>());
	panels_.emplace("Console", std::make_unique<ConsolePanel>());
}


/////////////////////////////////////////////////////////////////////////////////////////
//		更新
/////////////////////////////////////////////////////////////////////////////////////////
void PanelController::RenderPanels() {
	for (const auto& panel : panels_) {
		// 描画フラグが立っていなければスキップ
		if (!panel.second->IsShow()) continue;
		panel.second->Render();
	}
}

void PanelController::RegisterPanel(const std::string name, std::unique_ptr<IEngineUI> panel) {
	panels_.emplace(name, std::move(panel));
}

#include "EditorPanel.h"
/* ========================================================================
/*	include space
/* ===================================================================== */
// engine
#include <Engine/Application/UI/EngineUI/SelectionManager.h>

// lib
#include <externals/imgui/imgui.h>

///////////////////////////////////////////////////////////////////////////
// static variable
///////////////////////////////////////////////////////////////////////////
int EditorPanel::selectedEditorIndex = -1;

///////////////////////////////////////////////////////////////////////////
// コンストラクタ
///////////////////////////////////////////////////////////////////////////
EditorPanel::EditorPanel()
	:IEngineUI("Editor"){}

///////////////////////////////////////////////////////////////////////////
// 描画関数
///////////////////////////////////////////////////////////////////////////
void EditorPanel::Render(){
	ImGui::Begin(panelName_.c_str());

	ImGui::Text("Editor List");

	//線を引く
	ImGui::Separator();
	ImGui::Dummy(ImVec2(0.0f, 5.0f));

	//追加されているエディタのリストを描画
	for (size_t i = 0; i < editors_.size(); i++){
		bool isSelected(selectedEditorIndex == static_cast< int >(i));
		if (ImGui::Selectable(editors_[i]->GetEditorName().c_str(),isSelected)){
			selectedEditorIndex = static_cast< int >(i);
			SelectionManager::GetInstance()->SetSelectedEditor(editors_[i]);
			// 選択をクリア
			SelectionManager::GetInstance()->ClearSelectedObject();
		}
		if (isSelected){
			ImGui::SetItemDefaultFocus();
		}
	}

	ImGui::End();
}

///////////////////////////////////////////////////////////////////////////
// パネル名の取得
///////////////////////////////////////////////////////////////////////////
const std::string& EditorPanel::GetPanelName() const{
	return panelName_;
}

///////////////////////////////////////////////////////////////////////////
// エディタの追加
///////////////////////////////////////////////////////////////////////////
void EditorPanel::AddEditor(const BaseEditor* editor){
	editors_.push_back(const_cast< BaseEditor* >(editor));
}

///////////////////////////////////////////////////////////////////////////
// エディタの削除
///////////////////////////////////////////////////////////////////////////
void EditorPanel::RemoveEditor(const BaseEditor* editor){
	editors_.erase(std::remove(editors_.begin(), editors_.end(), editor), editors_.end());
}

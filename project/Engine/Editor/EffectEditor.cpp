#include "EffectEditor.h"

/* ========================================================================
/*		include space
/* ===================================================================== */
// engine
#include <Game/Effect/ParticleEffect/ParticleEffectCollection.h>

// externals
#include <externals/imgui/imgui.h>

/////////////////////////////////////////////////////////////////////////////////////////
//		コンストラクタ
/////////////////////////////////////////////////////////////////////////////////////////
EffectEditor::EffectEditor(ParticleEffectCollection* collection)
	: effectCollection_(collection){
	editorName_ = "EffectEditor";
}

/////////////////////////////////////////////////////////////////////////////////////////
//		gui表示
/////////////////////////////////////////////////////////////////////////////////////////
void EffectEditor::ShowImGuiInterface(){
	ImGui::SeparatorText("New Effect");
	ShowParticleMakingGui();

	ShowEffectListAndProperty();
}

/////////////////////////////////////////////////////////////////////////////////////////
//		jsonファイルに保存
/////////////////////////////////////////////////////////////////////////////////////////
void EffectEditor::SaveToJson([[maybe_unused]] const std::string& filePath){}

/////////////////////////////////////////////////////////////////////////////////////////
//		jsonファイルから読み込み
/////////////////////////////////////////////////////////////////////////////////////////
void EffectEditor::LoadFromJson([[maybe_unused]] const std::string& filePath){}

/////////////////////////////////////////////////////////////////////////////////////////
//		particle作成gui
/////////////////////////////////////////////////////////////////////////////////////////
void EffectEditor::ShowParticleMakingGui(){
	static char effectName[128] = "";
	ImGui::InputText("Effect Name", effectName, sizeof(effectName));
	if (ImGui::Button("Create Effect")){
		auto newEffect = std::make_unique<ParticleEffect>();
		newEffect->AddParticle(std::make_unique<Particle>());
		newEffect->SetName(effectName);
		effectCollection_->AddEffect(std::move(newEffect));
		effectName[0] = '\0';
	}
}
/////////////////////////////////////////////////////////////////////////////////////////
//		effectリスト表示
/////////////////////////////////////////////////////////////////////////////////////////
void EffectEditor::ShowEffectList(){
	auto& effects = effectCollection_->GetEffects();
	for (int i = 0; i < effects.size(); ++i){
		bool isSelected = (selectedEffectIndex_ == static_cast< int >(i));
		ImGui::PushID(i);

		if (ImGui::Selectable((effects[i]->GetName() + std::to_string(i)).c_str(), isSelected)){
			selectedEffectIndex_ = i;
		}

		// 右クリックでポップアップメニューを開く
		if (ImGui::BeginPopupContextItem()){
			if (ImGui::MenuItem("Rename")){
				ImGui::OpenPopup("RenameEffectPopup");
			}
			if (ImGui::MenuItem("Delete")){
				RemoveEffect(i);
				selectedEffectIndex_ = -1;
				ImGui::EndPopup();
				ImGui::PopID();
				break;
			}
			ImGui::EndPopup();
		}

		// Rename 用ポップアップ
		if (ImGui::BeginPopup("RenameEffectPopup")){
			static char newName[128] = "";
			ImGui::InputText("New Name", newName, IM_ARRAYSIZE(newName));
			if (ImGui::Button("OK")){
				effects[i]->SetName(newName);
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel")){
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}

		ImGui::PopID();
	}
}

void EffectEditor::ShowEffectListAndProperty(){

	// 全体を水平に分割
	ImVec2 windowSize = ImGui::GetContentRegionAvail();
	float leftPaneWidth = windowSize.x * 0.3f;
	float rightPaneWidth = windowSize.x - leftPaneWidth - 8.0f; // Padding分引く

	// 左カラム: エフェクトリスト
	ImGui::BeginChild("Effect List Pane", ImVec2(leftPaneWidth, 0), true);
	ImGui::Text("Effect List");
	ImGui::Separator();
	auto& effects = effectCollection_->GetEffects();
	static int renameIndex = -1; // リネーム対象のインデックス
	static char newName[128] = "";

	for (int i = 0; i < effects.size(); ++i){
		bool isSelected = (selectedEffectIndex_ == static_cast< int >(i));
		ImGui::PushID(i);

		if (renameIndex == i){
			ImGui::SetNextItemWidth(-1);
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 2));
			if (ImGui::InputText("##RenameInput", newName, IM_ARRAYSIZE(newName), ImGuiInputTextFlags_EnterReturnsTrue)){
				effects[i]->SetName(newName);
				renameIndex = -1;
			}
			if (!ImGui::IsItemActive() && ImGui::IsMouseClicked(0)){
				renameIndex = -1;
			}
			ImGui::PopStyleVar();
		} else{
			if (ImGui::Selectable((effects[i]->GetName() + std::to_string(i)).c_str(), isSelected)){
				selectedEffectIndex_ = i;
			}
		}

		// 右クリックでポップアップメニューを開く
		if (ImGui::BeginPopupContextItem()){
			if (ImGui::MenuItem("Rename")){
				renameIndex = i;
				strncpy_s(newName, effects[i]->GetName().c_str(), sizeof(newName));
			}
			if (ImGui::MenuItem("Delete")){
				RemoveEffect(i);
				selectedEffectIndex_ = -1;
				ImGui::EndPopup();
				ImGui::PopID();
				break;
			}
			ImGui::EndPopup();
		}

		ImGui::PopID();
	}

	ImGui::EndChild();

	ImGui::SameLine();

	// 右カラム: 選択中のエフェクトのプロパティ
	ImGui::BeginChild("Effect Property Pane", ImVec2(rightPaneWidth, 0), true);
	ImGui::Text("Effect Properties");
	ImGui::Separator();
	if (selectedEffectIndex_ >= 0 && selectedEffectIndex_ < static_cast< int >(effectCollection_->GetEffects().size())){
		effectCollection_->GetEffects()[selectedEffectIndex_]->ImGui();
	}
	ImGui::EndChild();

}


/////////////////////////////////////////////////////////////////////////////////////////
//		effectプロパティ表示
/////////////////////////////////////////////////////////////////////////////////////////
void EffectEditor::ShowEffectProperty(){
	if (selectedEffectIndex_ >= 0 && selectedEffectIndex_ < static_cast< int >(effectCollection_->GetEffects().size())){
		effectCollection_->GetEffects()[selectedEffectIndex_]->ImGui();
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
//		effect追加
/////////////////////////////////////////////////////////////////////////////////////////
void EffectEditor::AddEffect(std::unique_ptr<ParticleEffect> effect){
	effectCollection_->AddEffect(std::move(effect));
}

/////////////////////////////////////////////////////////////////////////////////////////
//		effect削除
/////////////////////////////////////////////////////////////////////////////////////////
void EffectEditor::RemoveEffect(int index){ effectCollection_->RemoveEffect(index); }

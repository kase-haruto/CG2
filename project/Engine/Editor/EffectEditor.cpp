#include "EffectEditor.h"

/* ========================================================================
/*		include space
/* ===================================================================== */
// engine
#include <Game/Effect/ParticleEffect/ParticleEffectCollection.h>

// externals
#include <externals/imgui/imgui.h>

// c++
#include <filesystem>
namespace fs = std::filesystem;

/////////////////////////////////////////////////////////////////////////////////////////
//		コンストラクタ
/////////////////////////////////////////////////////////////////////////////////////////
EffectEditor::EffectEditor(){
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
void EffectEditor::SaveToJson([[maybe_unused]] const std::string& filePath){
	if (currentEffect_){
		currentEffect_->Save(filePath);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
//		jsonファイルから読み込み
/////////////////////////////////////////////////////////////////////////////////////////
void EffectEditor::LoadFromJson([[maybe_unused]] const std::string& filePath){
}

void EffectEditor::LoadFromJsonAll(const std::string& directoryPath){
	ParticleEffectCollection::GetInstance()->Clear();
	for (const auto& entry : fs::directory_iterator(directoryPath)){
		if (entry.is_regular_file() && entry.path().extension() == ".json"){
			auto effect = std::make_unique<ParticleEffect>();
			effect->Load(entry.path().string());
			ParticleEffectCollection::GetInstance()->AddEffect(std::move(effect));
		}
	}
}

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
		ParticleEffectCollection::GetInstance()->AddEffect(std::move(newEffect));
		effectName[0] = '\0';
	}
	ImGui::SameLine();
	//ロード
	if (ImGui::Button("Load Effect")){
		LoadFromJsonAll(directoryPath_);
	}
}
/////////////////////////////////////////////////////////////////////////////////////////
//		effectリスト表示
/////////////////////////////////////////////////////////////////////////////////////////
void EffectEditor::ShowEffectListAndProperty(){

	// 全体を水平に分割
	ImVec2 windowSize = ImGui::GetContentRegionAvail();
	float leftPaneWidth = windowSize.x * 0.3f;
	float rightPaneWidth = windowSize.x - leftPaneWidth - 8.0f; // Padding分引く

	// 左カラム: エフェクトリスト
	ImGui::BeginChild("Effect List Pane", ImVec2(leftPaneWidth, 0), true);
	ImGui::Text("Effect List");
	ImGui::Separator();
	auto& effects = ParticleEffectCollection::GetInstance()->GetEffects();
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
				currentEffect_ = effects[i].get();
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
	if (ImGui::Button("saveEffect")){
		const std::string filePath = directoryPath_ +"/" + currentEffect_->GetName() + ".json";

		SaveToJson(filePath);
	}
	ImGui::Separator();
	if (currentEffect_){
		currentEffect_->ImGui();
	}
	ImGui::EndChild();

}


/////////////////////////////////////////////////////////////////////////////////////////
//		effect追加
/////////////////////////////////////////////////////////////////////////////////////////
void EffectEditor::AddEffect(std::unique_ptr<ParticleEffect> effect){
	ParticleEffectCollection::GetInstance()->AddEffect(std::move(effect));
}

/////////////////////////////////////////////////////////////////////////////////////////
//		effect削除
/////////////////////////////////////////////////////////////////////////////////////////
void EffectEditor::RemoveEffect(int index){ ParticleEffectCollection::GetInstance()->RemoveEffect(index); }

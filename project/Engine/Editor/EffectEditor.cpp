#include "EffectEditor.h"

/* ========================================================================
/*		include space
/* ===================================================================== */
// engine
#include <Game/Effect/ParticleEffect/ParticleEffectSystem.h>
#include <Engine/Graphics/Descriptor/SrvLocator.h>

// externals
#include <externals/imgui/imgui.h>

// c++
#include <filesystem>
namespace fs = std::filesystem;

/////////////////////////////////////////////////////////////////////////////////////////
//		コンストラクタ
/////////////////////////////////////////////////////////////////////////////////////////
EffectEditor::EffectEditor() {
	editorName_ = "EffectEditor";
}

/////////////////////////////////////////////////////////////////////////////////////////
//		gui表示
/////////////////////////////////////////////////////////////////////////////////////////
void EffectEditor::ShowImGuiInterface() {
	ImGui::SeparatorText("New Effect");

	//srvの個数を表示
	ImGui::Text("srvCount: %d", SrvLocator::GetCurrentOffset());

	ShowParticleMakingGui();

	ShowEffectListAndProperty();
}

/////////////////////////////////////////////////////////////////////////////////////////
//		jsonファイルに保存
/////////////////////////////////////////////////////////////////////////////////////////
void EffectEditor::SaveToJson([[maybe_unused]] const std::string& filePath) {
	if (currentEffect_) {
		currentEffect_->Save(filePath);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
//		jsonファイルから読み込み
/////////////////////////////////////////////////////////////////////////////////////////
void EffectEditor::LoadFromJson([[maybe_unused]] const std::string& filePath) {}

void EffectEditor::LoadFromJsonAll(const std::string& directoryPath) {
	auto& collection = ParticleEffectSystem::GetInstance()->GetCollection();
	collection.Clear();

	for (const auto& entry : fs::directory_iterator(directoryPath)) {
		if (entry.is_regular_file() && entry.path().extension() == ".json") {
			auto effect = std::make_unique<ParticleEffect>();
			effect->Load(entry.path().string());
			collection.AddEffect(std::move(effect));
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
//		particle作成gui
/////////////////////////////////////////////////////////////////////////////////////////
void EffectEditor::ShowParticleMakingGui() {
	static char effectName[128] = "";
	ImGui::InputText("Effect Name", effectName, sizeof(effectName));
	if (ImGui::Button("Create Effect")) {
		// 新規エフェクト作成
		auto newEffect = std::make_unique<ParticleEffect>();
		newEffect->AddParticle(std::make_unique<Particle>());
		newEffect->SetName(effectName);
		newEffect->Initialize();
		newEffect->Play(Vector3::Zero, EmitType::Both);

		  // ① プロトタイプとして登録
		ParticleEffectSystem::GetInstance()->GetCollection().AddEffect(std::make_unique<ParticleEffect>(*newEffect));
		// 追加したエフェクトをプレビュー再生（activeEffects_に追加）
		ParticleEffectSystem::GetInstance()->PlayForEditorPreview(std::move(newEffect));

		// 追加された最新エフェクトを選択
		auto& effects = ParticleEffectSystem::GetInstance()->GetCollection().GetEffects();
		currentEffect_ = effects.back().get();
		selectedEffectIndex_ = static_cast<int>(effects.size() - 1);

		effectName[0] = '\0';  // 入力欄クリア
	}
	ImGui::SameLine();
	if (ImGui::Button("Load Effect")) {
		LoadFromJsonAll(directoryPath_);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
//		effectリスト表示
/////////////////////////////////////////////////////////////////////////////////////////
void EffectEditor::ShowEffectListAndProperty() {
	ImVec2 windowSize = ImGui::GetContentRegionAvail();
	float leftPaneWidth = windowSize.x * 0.3f;
	float rightPaneWidth = windowSize.x - leftPaneWidth - 8.0f;

	// 左カラム: エフェクトリスト
	ImGui::BeginChild("Effect List Pane", ImVec2(leftPaneWidth, 0), true);
	ImGui::Text("Effect List");
	ImGui::Separator();

	auto& effects = ParticleEffectSystem::GetInstance()->GetCollection().GetEffects();
	static int renameIndex = -1;
	static char newName[128] = "";

	for (int i = 0; i < effects.size(); ++i) {
		bool isSelected = (selectedEffectIndex_ == i);
		ImGui::PushID(i);

		if (renameIndex == i) {
			ImGui::SetNextItemWidth(-1);
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 2));
			if (ImGui::InputText("##RenameInput", newName, IM_ARRAYSIZE(newName), ImGuiInputTextFlags_EnterReturnsTrue)) {
				effects[i]->SetName(newName);
				renameIndex = -1;
			}
			if (!ImGui::IsItemActive() && ImGui::IsMouseClicked(0)) {
				renameIndex = -1;
			}
			ImGui::PopStyleVar();
		} else {
			if (ImGui::Selectable((effects[i]->GetName() + std::to_string(i)).c_str(), isSelected)) {
				currentEffect_ = effects[i].get();
				selectedEffectIndex_ = i;
			}
		}

		if (ImGui::BeginPopupContextItem()) {
			if (ImGui::MenuItem("Rename")) {
				renameIndex = i;
				strncpy_s(newName, effects[i]->GetName().c_str(), sizeof(newName));
			}
			if (ImGui::MenuItem("Delete")) {
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
	if (ImGui::Button("saveEffect")) {
		const std::string filePath = directoryPath_ + "/" + currentEffect_->GetName() + ".json";
		SaveToJson(filePath);
	}
	ImGui::Separator();
	if (currentEffect_) {
		currentEffect_->ImGui();
	}
	ImGui::EndChild();
}

/////////////////////////////////////////////////////////////////////////////////////////
//		effect追加
/////////////////////////////////////////////////////////////////////////////////////////
void EffectEditor::AddEffect(std::unique_ptr<ParticleEffect> effect) {
	ParticleEffectSystem::GetInstance()->GetCollection().AddEffect(std::move(effect));
}

/////////////////////////////////////////////////////////////////////////////////////////
//		effect削除
/////////////////////////////////////////////////////////////////////////////////////////
void EffectEditor::RemoveEffect(int index) {
	ParticleEffectSystem::GetInstance()->GetCollection().RemoveEffect(index);
}

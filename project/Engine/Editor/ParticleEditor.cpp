#include "ParticleEditor.h"

/* ========================================================================
/*		include space
/* ===================================================================== */

//engine
#include <Engine/objects/particle/ParticleManager.h>

//externals
#include <externals/imgui/imgui.h>

/////////////////////////////////////////////////////////////////////////////////////////
//		コンストラクタ
/////////////////////////////////////////////////////////////////////////////////////////
ParticleEditor::ParticleEditor(ParticleManager* container){
	editorName_ = "ParticleEditor";
	particleContainer_ = container;
}

/////////////////////////////////////////////////////////////////////////////////////////
//		main gui
/////////////////////////////////////////////////////////////////////////////////////////
void ParticleEditor::ShowImGuiInterface(){
#ifdef _DEBUG
	ImVec2 totalSize = ImGui::GetContentRegionAvail();
	float topPanelHeight = 120.0f; // 作成UIの高さ（固定）

	// === 上部：新規作成パネル（横いっぱい） ===
	ImGui::BeginChild("TopCreatePanel", ImVec2(totalSize.x, topPanelHeight), true);
	{
		ShowParticleMakingGui();
	}
	ImGui::EndChild();

	// === 下部：横分割（リスト + インスペクタ） ===
	ImVec2 bottomSize = ImVec2(totalSize.x, totalSize.y - topPanelHeight);
	float listPanelWidth = bottomSize.x * 0.25f;  // リストは小さめ
	float inspectorPanelWidth = bottomSize.x - listPanelWidth;

	ImGui::BeginChild("BottomPanel", bottomSize, false);
	{
		// 左：パーティクルリスト
		ImGui::BeginChild("ParticleList", ImVec2(listPanelWidth, bottomSize.y), true);
		{
			ShowParticleList();
		}
		ImGui::EndChild();

		ImGui::SameLine();

		// 右：インスペクタ
		ImGui::BeginChild("ParticleEdit", ImVec2(inspectorPanelWidth, bottomSize.y), true);
		{
			ShowParticleProperty();
		}
		ImGui::EndChild();
	}
	ImGui::EndChild();

#endif // _DEBUG
}


/////////////////////////////////////////////////////////////////////////////////////////
//		jsonファイルに保存
/////////////////////////////////////////////////////////////////////////////////////////
void ParticleEditor::SaveToJson([[maybe_unused]] const std::string& filePath){}

/////////////////////////////////////////////////////////////////////////////////////////
//		jsonファイルから読み込み
/////////////////////////////////////////////////////////////////////////////////////////
void ParticleEditor::LoadFromJson([[maybe_unused]] const std::string& filePath){}

/////////////////////////////////////////////////////////////////////////////////////////
//		particleの編集
/////////////////////////////////////////////////////////////////////////////////////////
void ParticleEditor::ShowParticleMakingGui(){
	// 名前入力用バッファ（固定サイズ）
	static char particleNameCStr[128] = "";

	// 有効な名前（1文字以上）
	bool hasValidName = (std::strlen(particleNameCStr) > 1);

	ImGui::Text("Create New Particle");

	// 名前入力フィールド
	ImGui::InputText("Particle Name", particleNameCStr, sizeof(particleNameCStr));

	if (hasValidName){
		if (ImGui::Button("Create")){
			// 新しいパーティクルを作成（名前だけ設定）
			auto newParticle = std::make_unique<Particle>();
			newParticle->SetName(particleNameCStr);

			// 仮モデル/テクスチャを初期化
			const std::string defaultModelName = "plane.obj";
			const std::string defaultTextureName = "particle.png";
			newParticle->Initialize(defaultModelName, defaultTextureName);

			// リストへ登録（所有権を渡す）
			AddParticle(std::move(newParticle));

			// 入力をリセット
			particleNameCStr[0] = '\0';
		}
	} else{
		// エラーヒント表示
		if (std::strlen(particleNameCStr) == 1){
			ImGui::TextColored(ImVec4(1, 1, 0.5f, 1), "Please enter a name longer than 1 character.");
		} else{
			ImGui::TextColored(ImVec4(1, 0.5f, 0.5f, 1), "Please enter a name to continue.");
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
//		editor上のparticleのリストを表示
/////////////////////////////////////////////////////////////////////////////////////////
void ParticleEditor::ShowParticleList(){
	ImGui::Text("Select :");

	auto& particles = particleContainer_->GetParticles();

	// 状態管理
	static int renameIndex = -1;
	static char renameBuffer[128] = "";
	int deleteIndex = -1;

	for (size_t i = 0; i < particles.size(); ++i){
		Particle* particle = particles[i].get();
		std::string name = particle->GetName();
		if (name.empty()) name = "Unnamed";

		bool isSelected = (selectedParticleIndex_ == static_cast< int >(i));
		ImGui::PushID(static_cast< int >(i));

		// ==== リネーム状態中 ====
		if (renameIndex == static_cast< int >(i)){
			ImGui::SetNextItemWidth(160.0f);
			ImGui::SetKeyboardFocusHere(); // 自動で入力フォーカス
			if (ImGui::InputText("##Rename", renameBuffer, sizeof(renameBuffer), ImGuiInputTextFlags_EnterReturnsTrue)){
				// Enterキーで確定
				particle->SetName(renameBuffer);
				renameIndex = -1;
			}
			// フォーカスが外れたところでクリックしたらキャンセル
			if (!ImGui::IsItemActive() && !ImGui::IsItemHovered()){
				if (ImGui::IsMouseClicked(0)){
					renameIndex = -1;
				}
			}
		}

		// ==== 通常表示 ====
		else{
			if (ImGui::Selectable((name + "##label").c_str(), isSelected)){
				selectedParticleIndex_ = static_cast< int >(i);
			}

			// 右クリックメニュー
			if (ImGui::BeginPopupContextItem("ParticleContextMenu")){
				if (ImGui::MenuItem("Rename")){
					renameIndex = static_cast< int >(i);
					strncpy_s(renameBuffer, sizeof(renameBuffer), name.c_str(), _TRUNCATE);
				}
				if (ImGui::MenuItem("Delete")){
					deleteIndex = static_cast< int >(i);
				}
				ImGui::EndPopup();
			}
		}

		ImGui::PopID();
	}

	//削除
	if (deleteIndex != -1){
		RemoveParticle(deleteIndex);
		if (selectedParticleIndex_ == deleteIndex){
			selectedParticleIndex_ = -1;
		} else if (selectedParticleIndex_ > deleteIndex){
			--selectedParticleIndex_;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
//		編集中のparticleのプロパティを表示
/////////////////////////////////////////////////////////////////////////////////////////
void ParticleEditor::ShowParticleProperty(){
	ImGui::Text("Particle Properties");

	auto& particles = particleContainer_->GetParticles();

	// 選択されていない場合
	if (selectedParticleIndex_ < 0 || selectedParticleIndex_ >= static_cast< int >(particles.size())){
		ImGui::TextColored(ImVec4(1, 1, 0.5f, 1), "No particle selected.");
		return;
	}

	Particle* particle = particles[selectedParticleIndex_].get();
	ImGui::Separator();

	particle->ImGui(); // ParticleクラスのImGui関数を呼び出す

}


/////////////////////////////////////////////////////////////////////////////////////////
//		particleの追加
/////////////////////////////////////////////////////////////////////////////////////////
void ParticleEditor::AddParticle(std::unique_ptr<Particle> particle){
	particleContainer_->AddParticle(std::move(particle));
}

/////////////////////////////////////////////////////////////////////////////////////////
//		particleの削除
/////////////////////////////////////////////////////////////////////////////////////////
void ParticleEditor::RemoveParticle(int index){
	particleContainer_->RemoveParticleAt(index);
}

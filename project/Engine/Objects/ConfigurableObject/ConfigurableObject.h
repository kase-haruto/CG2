#pragma once
#include <Engine/Objects/3D/Actor/SceneObject.h>
#include <Engine/Objects/ConfigurableObject/IConfigurable.h>
#include <Engine/Foundation/Json/JsonUtils.h>

#include <externals/imgui/imgui.h>
#include <externals/imgui/ImGuiFileDialog.h>

template<typename TConfig>
class ConfigurableObject
	: public IConfigurable {
public:
	//* apply/extract ========================================//
	void ApplyConfigFromJson(const nlohmann::json& j) override;
	void ExtractConfigToJson(nlohmann::json& j) const override;

	//* save/load ============================================//
	void LoadConfig(const std::string& path);
	void SaveConfig(const std::string& path) const;

	void ShowGUi();

	virtual void ApplyConfig() = 0;   //< config_ → 実行状態へ反映
	virtual void ExtractConfig() = 0; //< 実行状態 → config_ へ反映

protected:
	TConfig config_;
};

/////////////////////////////////////////////////////////////////////////////////////////
//      jsonからコンフィグを適用
/////////////////////////////////////////////////////////////////////////////////////////
template<typename TConfig>
inline void ConfigurableObject<TConfig>::ApplyConfigFromJson(const nlohmann::json& j) {
	config_ = j.get<TConfig>();
	ApplyConfig();
}

/////////////////////////////////////////////////////////////////////////////////////////
//      コンフィグをjsonに変換
/////////////////////////////////////////////////////////////////////////////////////////
template<typename TConfig>
inline void ConfigurableObject<TConfig>::ExtractConfigToJson(nlohmann::json& j) const {
	const_cast<ConfigurableObject*>(this)->ExtractConfig();  // 状態を config_ に反映
	j = config_;
}

/////////////////////////////////////////////////////////////////////////////////////////
//      コンフィグのロード
/////////////////////////////////////////////////////////////////////////////////////////
template<typename TConfig>
inline void ConfigurableObject<TConfig>::LoadConfig(const std::string& path) {
	nlohmann::json j;
	if (JsonUtils::Load(path, j)) ApplyConfigFromJson(j);
}

/////////////////////////////////////////////////////////////////////////////////////////
//      コンフィグのセーブ
/////////////////////////////////////////////////////////////////////////////////////////
template<typename TConfig>
inline void ConfigurableObject<TConfig>::SaveConfig(const std::string& path) const {
	const_cast<ConfigurableObject*>(this)->ExtractConfig(); // mutable化して状態を config_ にコピー
	nlohmann::json j;
	j = config_;
	JsonUtils::Save(path, j);
}

/////////////////////////////////////////////////////////////////////////////////////////
//      コンフィグのgui
/////////////////////////////////////////////////////////////////////////////////////////
template<typename TConfig>
inline void ConfigurableObject<TConfig>::ShowGUi() {
	if (ImGui::Button("Load Config")) {
		IGFD::FileDialogConfig config;
		config.path = "Resources/Assets/Configs/";
		ImGuiFileDialog::Instance()->OpenDialog(
			"ConfigLoadDialog",
			"Load Config File",
			".json",
			config
		);
	}
	ImGui::SameLine();
	if (ImGui::Button("Save Config")) {
		IGFD::FileDialogConfig config;
		config.path = "Resources/Assets/Configs/";
		ImGuiFileDialog::Instance()->OpenDialog(
			"ConfigSaveDialog",
			"Save Config File",
			".json",
			config
		);
	}

	// Load ダイアログ処理
	if (ImGuiFileDialog::Instance()->Display("ConfigLoadDialog")) {
		if (ImGuiFileDialog::Instance()->IsOk()) {
			std::string filePath = ImGuiFileDialog::Instance()->GetFilePathName();
			LoadConfig(filePath);
		}
		ImGuiFileDialog::Instance()->Close();
	}

	// Save ダイアログ処理
	if (ImGuiFileDialog::Instance()->Display("ConfigSaveDialog")) {
		if (ImGuiFileDialog::Instance()->IsOk()) {
			std::string filePath = ImGuiFileDialog::Instance()->GetFilePathName();
			SaveConfig(filePath);
		}
		ImGuiFileDialog::Instance()->Close();
	}
}
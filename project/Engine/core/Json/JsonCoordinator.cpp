#include "JsonCoordinator.h"
#include <externals/imgui/imgui.h>
#include <iostream>


//-------------------------------------------------------------------
// ディレクトリ作成
//-------------------------------------------------------------------
void JsonCoordinator::EnsureDirectoryExists(const std::string& path){
	std::filesystem::create_directories(std::filesystem::path(path).parent_path());
}

//-------------------------------------------------------------------
// ファイルパス生成 (必要に応じてカスタマイズ)
//-------------------------------------------------------------------
std::string JsonCoordinator::MakeFilePath(const std::string& group){
	// グループ名をファイル名にしてみる（例: "resources/json/GroupA/GroupA.json"）
	s_baseDirectory_ = "resources/json/";
	return s_baseDirectory_ + group + "/" + group + ".json";
}

//-------------------------------------------------------------------
// 値を設定
//-------------------------------------------------------------------
bool JsonCoordinator::SetValue(const std::string& group, const std::string& key, AdjustableValue value){
	// グループ or キーが未登録なら失敗
	if (!s_groupData_.count(group)){
		return false;
	}
	if (!s_groupData_[group].contains(key)){
		return false;
	}

	// データを上書き
	s_groupData_[group][key] = value;

	// バインドがあれば反映
	if (s_bindings_.count(group) && s_bindings_[group].count(key)){
		s_bindings_[group][key](value);
	}
	return true;
}

//-------------------------------------------------------------------
// 値を取得
//-------------------------------------------------------------------
std::optional<AdjustableValue> JsonCoordinator::GetValue(const std::string& group, const std::string& key){
	if (s_groupData_.count(group) && s_groupData_[group].contains(key)){
		return s_groupData_[group][key].get<AdjustableValue>();
	}
	return std::nullopt;
}

//-------------------------------------------------------------------
// グループだけを保存（ファイル分割方式）
//-------------------------------------------------------------------
bool JsonCoordinator::SaveGroup(const std::string& group){
	if (!s_groupData_.count(group)){
		// グループが何も登録されていない
		return false;
	}

	std::string path = MakeFilePath(group);
	EnsureDirectoryExists(path);

	std::ofstream ofs(path);
	if (!ofs.is_open()){
		return false;
	}

	// グループ分の JSON を書き込む
	ofs << s_groupData_[group].dump(4);
	ofs.close();
	return true;
}

//-------------------------------------------------------------------
// グループだけをロード（ファイル分割方式）
//-------------------------------------------------------------------
bool JsonCoordinator::LoadGroup(const std::string& group){
	std::string path = MakeFilePath(group);
	if (!std::filesystem::exists(path)){
		// もしファイルが無い場合は、新規として保存して終わる
		// （あるいは失敗として return false;）
		SaveGroup(group);
		return false;
	}

	std::ifstream ifs(path);
	if (!ifs.is_open()){
		return false;
	}

	json j;
	ifs >> j;
	ifs.close();

	// s_groupData_[group] に読み込んだデータを丸ごと上書き
	s_groupData_[group] = j;

	// バインドされている変数に反映
	for (auto&& [key, val] : j.items()){
		if (s_bindings_.count(group) && s_bindings_[group].count(key)){
			s_bindings_[group][key](val.get<AdjustableValue>());
		}
	}

	return true;
}

//-------------------------------------------------------------------
// グループ内アイテムをImGuiでレンダリング
//-------------------------------------------------------------------
void JsonCoordinator::RenderAdjustableItem(const std::string& group, const std::string& key){
	if (auto opt = GetValue(group, key)){
		auto& value = *opt;
		if (std::holds_alternative<int>(value)){
			int val = std::get<int>(value);
			if (ImGui::InputInt(key.c_str(), &val)){
				SetValue(group, key, val);
			}
		} else if (std::holds_alternative<float>(value)){
			float val = std::get<float>(value);
			if (ImGui::InputFloat(key.c_str(), &val)){
				SetValue(group, key, val);
			}
		} else if (std::holds_alternative<Vector3>(value)){
			Vector3 v = std::get<Vector3>(value);
			if (ImGui::DragFloat3(key.c_str(), &v.x, 0.01f)){
				SetValue(group, key, v);
			}
		}
	}
}

//-------------------------------------------------------------------
// グループ内のすべての項目をレンダリング
//-------------------------------------------------------------------
void JsonCoordinator::RenderGroupUI(const std::string& group){
	if (!s_groupData_.count(group)){
		ImGui::Text("No data for group: %s", group.c_str());
		return;
	}
	for (auto&& [key, val] : s_groupData_[group].items()){
		RenderAdjustableItem(group, key);
	}
}

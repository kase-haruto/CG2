#include "JsonCoordinator.h"
#include <externals/imgui/imgui.h>
#include <iostream>
#include <stdexcept>

std::string JsonCoordinator::s_baseDirectory_ = "Resources/json/";

//-------------------------------------------------------------------
// ディレクトリ作成
//-------------------------------------------------------------------
void JsonCoordinator::EnsureDirectoryExists(const std::string& path){
    try{
        std::filesystem::create_directories(std::filesystem::path(path).parent_path());
    } catch (const std::exception& e){
        std::cerr << "Error creating directory: " << e.what() << std::endl;
        throw;
    }
}

//-------------------------------------------------------------------
// ファイルパス生成 (必要に応じてカスタマイズ)
//-------------------------------------------------------------------
std::string JsonCoordinator::MakeFilePath(const std::string& group, std::optional<std::string> directoryPath){
    std::string baseDirectory;

    // directoryPath が指定されている場合
    if (directoryPath.has_value()){
        std::filesystem::path customPath = directoryPath.value();
        // フルパスかどうかを確認
        if (customPath.is_absolute()){
            baseDirectory = customPath.string(); // フルパスならそのまま使用
        } else{
            // 相対パスの場合は s_baseDirectory_ を基準に結合
            baseDirectory = (std::filesystem::path(s_baseDirectory_) / customPath).string();
        }
    } else{
        // directoryPath が指定されていなければ s_baseDirectory_ を使用
        baseDirectory = s_baseDirectory_;
    }

    // 最終的なパスを生成して返す
    return (std::filesystem::path(baseDirectory) / group / (group + ".json")).string();
}

//-------------------------------------------------------------------
// 値を設定
//-------------------------------------------------------------------
bool JsonCoordinator::SetValue(const std::string& group, const std::string& key, AdjustableValue value){
    if (!s_groupData_.count(group) || !s_groupData_[group].contains(key)){
        return false;
    }

    s_groupData_[group][key] = value;

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
bool JsonCoordinator::SaveGroup(const std::string& group, std::optional<std::string> directoryPath){
    if (!s_groupData_.count(group)){
        return false;
    }

    std::string path = MakeFilePath(group, directoryPath);
    EnsureDirectoryExists(path);

    try{
        std::ofstream ofs(path);
        if (!ofs.is_open()){
            return false;
        }

        ofs << s_groupData_[group].dump(4);
        ofs.close();
    } catch (const std::exception& e){
        std::cerr << "Error saving group: " << e.what() << std::endl;
        return false;
    }

    return true;
}

//-------------------------------------------------------------------
// グループだけをロード（ファイル分割方式）
//-------------------------------------------------------------------
bool JsonCoordinator::LoadGroup(const std::string& group, std::optional<std::string> directoryPath){
    std::string path = MakeFilePath(group, directoryPath);

    if (!std::filesystem::exists(path)){
        SaveGroup(group, directoryPath); // 新規保存
        return false;
    }

    try{
        std::ifstream ifs(path);
        if (!ifs.is_open()){
            return false;
        }

        json j;
        ifs >> j;
        ifs.close();

        s_groupData_[group] = j;

        for (auto&& [key, val] : j.items()){
            if (s_bindings_.count(group) && s_bindings_[group].count(key)){
                s_bindings_[group][key](val.get<AdjustableValue>());
            }
        }
    } catch (const std::exception& e){
        std::cerr << "Error loading group: " << e.what() << std::endl;
        return false;
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

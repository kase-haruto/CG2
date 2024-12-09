#include "JsonCoordinator.h"
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <iostream>
#include <externals/imgui/imgui.h>

//-------------------------------------------------------------------
// ヘルパー関数: フルパスを構築
//-------------------------------------------------------------------
std::string JsonCoordinator::ConstructFullPath(const std::string& fileName, const std::optional<std::string>& parentPath){
    std::string fullPath = baseDirectory_;
    if (parentPath){
        fullPath += *parentPath + "/";
    }
    fullPath += fileName + ".json";
    return fullPath;
}

//-------------------------------------------------------------------
// 値を設定する
//-------------------------------------------------------------------
void JsonCoordinator::SetValue(const std::string& group, const std::string& key, AdjustableValue value){
    if (data_.count(group) && data_[group].count(key)){
        data_[group][key] = value;

        // バインディングが存在する場合はコールバックを呼び出す
        if (bindings_.count(group) && bindings_[group].count(key)){
            bindings_[group][key](value);
        }
    } else{
        throw std::runtime_error("Key not registered: " + group + "::" + key);
    }
}

//-------------------------------------------------------------------
// 値を取得する
//-------------------------------------------------------------------
AdjustableValue JsonCoordinator::GetValue(const std::string& group, const std::string& key){
    if (data_.count(group) && data_[group].count(key)){
        return data_.at(group).at(key);
    }
    throw std::runtime_error("Key not found: " + group + "::" + key);
}

//-------------------------------------------------------------------
// JSONとして保存
//-------------------------------------------------------------------
void JsonCoordinator::Save(const std::string& fileName, std::optional<std::string> parentPath){
    std::string fullPath = ConstructFullPath(fileName, parentPath);

    // 必要なディレクトリを作成
    std::filesystem::create_directories(std::filesystem::path(fullPath).parent_path());

    // データを JSON に変換
    json jsonData = data_;

    // ファイルに書き込む
    std::ofstream file(fullPath);
    if (!file.is_open()){
        throw std::runtime_error("Could not open file for saving: " + fullPath);
    }
    file << jsonData.dump(4);  // インデントを 4 に設定
    file.close();

    std::cout << "Data saved successfully to: " << fullPath << std::endl;
}

//-------------------------------------------------------------------
// JSONからロード
//-------------------------------------------------------------------
void JsonCoordinator::Load(const std::string& fileName, std::optional<std::string> parentPath){
    std::string fullPath = ConstructFullPath(fileName, parentPath);

    // ファイルが存在しない場合、新規作成
    if (!std::filesystem::exists(fullPath)){
        std::cout << "File does not exist. Creating a new file: " << fullPath << std::endl;
        Save(fileName, parentPath);
        return;
    }

    // ファイルを開く
    std::ifstream file(fullPath);
    if (!file.is_open()){
        throw std::runtime_error("Could not open file for loading: " + fullPath);
    }

    // JSON を読み込み
    json jsonData;
    file >> jsonData;
    file.close();

    // JSON データを `data_` に変換
    try{
        data_ = jsonData.get<decltype(data_)>();
    } catch (const std::exception& e){
        throw std::runtime_error("Failed to parse JSON data: " + std::string(e.what()));
    }

    // バインドされている変数に反映
    for (const auto& [group, items] : data_){
        for (const auto& [key, value] : items){
            if (bindings_.count(group) && bindings_[group].count(key)){
                bindings_[group][key](value);
            }
        }
    }

    std::cout << "Data loaded successfully from: " << fullPath << std::endl;
}

//-------------------------------------------------------------------
// 個別アイテムをレンダリング
//-------------------------------------------------------------------
void JsonCoordinator::RenderAdjustableItem(const std::string& group, const std::string& key){
    try{
        AdjustableValue value = GetValue(group, key);

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
        } else{
            ImGui::Text("Unsupported type for key: %s", key.c_str());
        }
    } catch (const std::exception& e){
        ImGui::Text("Error: %s", e.what());
    }
}

//-------------------------------------------------------------------
// グループ内のすべての項目をレンダリング
//-------------------------------------------------------------------
void JsonCoordinator::RenderGroupUI(const std::string& group){
    if (data_.count(group) == 0){
        ImGui::Text("No data for group: %s", group.c_str());
        return;
    }

    for (const auto& [key, value] : data_[group]){
        RenderAdjustableItem(group, key);
    }
}

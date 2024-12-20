#include "JsonCoordinator.h"
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
// ヘルパー関数: ディレクトリを確保
//-------------------------------------------------------------------
void JsonCoordinator::EnsureDirectoryExists(const std::string& path){
    std::filesystem::create_directories(std::filesystem::path(path).parent_path());
}

//-------------------------------------------------------------------
// 値を設定する
//-------------------------------------------------------------------
bool JsonCoordinator::SetValue(const std::string& group, const std::string& key, AdjustableValue value){
    if (!data_.contains(group) || !data_[group].contains(key)){
        return false; // 未登録のキー
    }

    data_[group][key] = value;

    // バインディングが存在する場合はコールバックを呼び出す
    if (bindings_.count(group) && bindings_[group].count(key)){
        bindings_[group][key](value);
    }
    return true;
}

//-------------------------------------------------------------------
// 値を取得する
//-------------------------------------------------------------------
std::optional<AdjustableValue> JsonCoordinator::GetValue(const std::string& group, const std::string& key){
    if (data_.contains(group) && data_[group].contains(key)){
        return data_[group][key].get<AdjustableValue>();
    }
    return std::nullopt; // エラー時は無効な値を返す
}

//-------------------------------------------------------------------
// JSONとして保存
//-------------------------------------------------------------------
bool JsonCoordinator::Save(const std::string& fileName, std::optional<std::string> parentPath){
    std::string fullPath = ConstructFullPath(fileName, parentPath);
    EnsureDirectoryExists(fullPath);

    std::ofstream file(fullPath);
    if (!file.is_open()){
        return false; // 保存に失敗
    }
    file << data_.dump(4); // インデントを 4 に設定
    file.close();

    return true; // 成功
}

//-------------------------------------------------------------------
// JSONからロード
//-------------------------------------------------------------------
bool JsonCoordinator::Load(const std::string& fileName, std::optional<std::string> parentPath){
    std::string fullPath = ConstructFullPath(fileName, parentPath);

    if (!std::filesystem::exists(fullPath)){
        Save(fileName, parentPath); // 新規作成
        return false;
    }

    std::ifstream file(fullPath);
    if (!file.is_open()){
        return false; // ロード失敗
    }

    json jsonData;
    file >> jsonData;
    file.close();

    data_ = jsonData;

    // バインドされている変数に反映
    for (const auto& [group, items] : data_.items()){
        for (const auto& [key, value] : items.items()){
            if (bindings_.count(group) && bindings_[group].count(key)){
                bindings_[group][key](value.get<AdjustableValue>());
            }
        }
    }

    return true; // 正常終了
}

//-------------------------------------------------------------------
// 個別アイテムをレンダリング
//-------------------------------------------------------------------
void JsonCoordinator::RenderAdjustableItem(const std::string& group, const std::string& key){
    if (auto value = GetValue(group, key)){
        if (std::holds_alternative<int>(*value)){
            int val = std::get<int>(*value);
            if (ImGui::InputInt(key.c_str(), &val)){
                SetValue(group, key, val);
            }
        } else if (std::holds_alternative<float>(*value)){
            float val = std::get<float>(*value);
            if (ImGui::InputFloat(key.c_str(), &val)){
                SetValue(group, key, val);
            }
        } else if (std::holds_alternative<Vector3>(*value)){
            Vector3 v = std::get<Vector3>(*value);
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
    if (!data_.contains(group)){
        ImGui::Text("No data for group: %s", group.c_str());
        return;
    }

    for (const auto& [key, value] : data_[group].items()){
        RenderAdjustableItem(group, key);
    }
}
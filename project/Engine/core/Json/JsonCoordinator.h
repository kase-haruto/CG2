#pragma once

#include <string>
#include <unordered_map>
#include <variant>
#include <functional>
#include <optional>
#include <stdexcept>
#include <fstream>
#include <filesystem>

#include <nlohmann/json.hpp>
#include "lib/myMath/Vector3.h"

using json = nlohmann::json;

// AdjustableValue: 管理するデータ型
using AdjustableValue = std::variant<int, float, Vector3>;

class JsonCoordinator{
public:
    //===================================================================*/
    //                   public function
    //===================================================================*/
    // 項目を登録してバインド
    template <typename T>
    static bool RegisterItem(const std::string& group, const std::string& key, T& target);

    // 値を設定
    static bool SetValue(const std::string& group, const std::string& key, AdjustableValue value);

    // 値を取得
    static std::optional<AdjustableValue> GetValue(const std::string& group, const std::string& key);

    // データを保存
    static bool Save(const std::string& fileName, std::optional<std::string> parentPath = std::nullopt);

    // データをロード
    static bool Load(const std::string& fileName, std::optional<std::string> parentPath = std::nullopt);

    // グループ内のすべての項目をレンダリング
    static void RenderGroupUI(const std::string& group);

private:
    //===================================================================*/
    //                   private function
    //===================================================================*/
    // 個別アイテムをレンダリング
    static void RenderAdjustableItem(const std::string& group, const std::string& key);

    // フルパスを構築
    static std::string ConstructFullPath(const std::string& fileName, const std::optional<std::string>& parentPath);

    // ディレクトリ作成
    static void EnsureDirectoryExists(const std::string& path);

private:
    //===================================================================*/
    //                   private variable
    //===================================================================*/
    static inline std::string baseDirectory_ = "resources/json/";  // ベースディレクトリ
    static inline std::unordered_map<std::string, std::unordered_map<std::string, AdjustableValue>> data_;  // グループ -> (キー -> 値)
    static inline std::unordered_map<std::string, std::unordered_map<std::string, std::function<void(const AdjustableValue&)>>> bindings_;  // グループ -> (キー -> バインディングコールバック)
};

//===================================================================*/
//                  inline Function
//===================================================================*/

// Vector3 を JSON 形式に変換
inline void to_json(json& j, const Vector3& v){
    j = json {{"x", v.x}, {"y", v.y}, {"z", v.z}};
}

// JSON から Vector3 に変換
inline void from_json(const json& j, Vector3& v){
    v.x = j.at("x").get<float>();
    v.y = j.at("y").get<float>();
    v.z = j.at("z").get<float>();
}

// AdjustableValue を JSON 形式に変換
inline void to_json(json& j, const AdjustableValue& value){
    std::visit([&] (const auto& arg){ j = arg; }, value);
}

// JSON から AdjustableValue に変換
inline void from_json(const json& j, AdjustableValue& value){
    if (j.is_number_integer()){
        value = j.get<int>();
    } else if (j.is_number_float()){
        value = j.get<float>();
    } else if (j.is_object()){
        value = j.get<Vector3>();
    }
}

//===================================================================*/
//                  template Function
//===================================================================*/
template <typename T>
bool JsonCoordinator::RegisterItem(const std::string& group, const std::string& key, T& target){
    if (data_[group].count(key) > 0){
        return false; // 既に登録済み
    }

    data_[group][key] = target;

    bindings_[group][key] = [&target] (const AdjustableValue& value){
        if (auto val = std::get_if<T>(&value)){
            target = *val; // 型が一致している場合のみ代入
        }
        };

    // 登録済みの値を変数に同期
    if (data_.count(group) && data_[group].count(key)){
        if (auto val = std::get_if<T>(&data_[group][key])){
            target = *val;
        }
    }
    return true;
}

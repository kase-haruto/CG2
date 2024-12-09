#pragma once

#include <string>
#include <unordered_map>
#include <variant>
#include <functional>
#include <optional>
#include <stdexcept>
#include <fstream>

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
    // ベースディレクトリを設定
    static void SetBaseDirectory(const std::string& baseDir){
        baseDirectory_ = baseDir;
    }

    // 項目を登録してバインド
    template <typename T>
    static void RegisterItem(const std::string& group, const std::string& key, T& target);

    // 値を設定
    static void SetValue(const std::string& group, const std::string& key, AdjustableValue value);

    // 値を取得
    static AdjustableValue GetValue(const std::string& group, const std::string& key);

    // データを保存
    static void Save(const std::string& fileName, std::optional<std::string> parentPath = std::nullopt);

    // データをロード
    static void Load(const std::string& fileName, std::optional<std::string> parentPath = std::nullopt);

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
    } else{
        throw std::runtime_error("Unsupported type for AdjustableValue");
    }
}

//===================================================================*/
//                  template Function
//===================================================================*/
template <typename T>
void JsonCoordinator::RegisterItem(const std::string& group, const std::string& key, T& target){
    try{
        // 初期値を設定
        data_[group][key] = target;

        // バインド設定
        bindings_[group][key] = [&target, key] (const AdjustableValue& value){ // key をコピーキャプチャ
            try{
                target = std::get<T>(value);
            } catch (const std::bad_variant_access&){
                throw std::runtime_error("Type mismatch for key: " + key);
            }
            };

        // 登録済みの値を変数に同期
        if (data_.count(group) && data_[group].count(key)){
            target = std::get<T>(data_[group][key]);
        }
    } catch (const std::exception& e){
        throw std::runtime_error("Failed to register item: " + group + "::" + key + " - " + e.what());
    }
}




#pragma once

#include <string>
#include <variant>
#include <functional>
#include <optional>
#include <stdexcept>
#include <fstream>
#include <filesystem>
#include <unordered_map>
#include <externals/nlohmann/json.hpp>
#include "lib/myMath/Vector3.h"

using json = nlohmann::ordered_json;
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

    // グループのみ保存 (ファイル分割方式)
    static bool SaveGroup(const std::string& group);

    // グループのみロード (ファイル分割方式)
    static bool LoadGroup(const std::string& group);

    // グループ内のすべての項目をレンダリング (ImGui)
    static void RenderGroupUI(const std::string& group);

private:
    //===================================================================*/
    //                   private function
    //===================================================================*/
    static void RenderAdjustableItem(const std::string& group, const std::string& key);

    // ディレクトリ作成
    static void EnsureDirectoryExists(const std::string& path);

    // グループ -> JSON ファイル名を作る（適宜カスタマイズ）
    static std::string MakeFilePath(const std::string& group);

private:
    //===================================================================*/
    //                   private variable
    //===================================================================*/
    // グループごとに独立した JSON データを保管
    static inline std::unordered_map<std::string, json> s_groupData_;

    // バインディング: グループ -> (キー -> コールバック)
    static inline std::unordered_map<std::string,
        std::unordered_map<std::string, std::function<void(const AdjustableValue&)>>>
        s_bindings_;

    // ベースパスなど
    static inline std::string s_baseDirectory_;
};

//-------------------------------------------------------------------
// Vector3 を JSON 形式に変換
//-------------------------------------------------------------------
inline void to_json(json& j, const Vector3& v){
    j = json {{"x", v.x}, {"y", v.y}, {"z", v.z}};
}

//-------------------------------------------------------------------
// JSON から Vector3 に変換
//-------------------------------------------------------------------
inline void from_json(const json& j, Vector3& v){
    v.x = j.at("x").get<float>();
    v.y = j.at("y").get<float>();
    v.z = j.at("z").get<float>();
}

//-------------------------------------------------------------------
// AdjustableValue を JSON 形式に変換
//-------------------------------------------------------------------
inline void to_json(json& j, const AdjustableValue& value){
    std::visit([&] (auto&& arg){ j = arg; }, value);
}

//-------------------------------------------------------------------
// JSON から AdjustableValue に変換
//-------------------------------------------------------------------
inline void from_json(const json& j, AdjustableValue& value){
    if (j.is_number_integer()){
        value = j.get<int>();
    } else if (j.is_number_float()){
        value = j.get<float>();
    } else if (j.is_object()){
        value = j.get<Vector3>();
    }
}

//-------------------------------------------------------------------
// RegisterItem (テンプレート)
//-------------------------------------------------------------------
template <typename T>
bool JsonCoordinator::RegisterItem(const std::string& group, const std::string& key, T& target){
    // まだグループが存在しなければ空の json を確保
    if (!s_groupData_.count(group)){
        s_groupData_[group] = json::object();
    }

    // すでに登録済みなら失敗とする例
    if (s_groupData_[group].contains(key)){
        return false;
    }

    // データに登録
    s_groupData_[group][key] = target;

    // バインディング登録
    s_bindings_[group][key] = [&target] (const AdjustableValue& value){
        if (auto valPtr = std::get_if<T>(&value)){
            target = *valPtr;
        }
        };

    // データを変数に同期
    // ※現状だと既に書き込んでいるので実質不要だが念のため
    auto val = s_groupData_[group][key].get<AdjustableValue>();
    if (auto valPtr = std::get_if<T>(&val)){
        target = *valPtr;
    }

    return true;
}
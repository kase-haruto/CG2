#pragma once

#include <string>
#include <unordered_map>
#include <variant>
#include <nlohmann/json.hpp>


using json = nlohmann::json;
using AdjustableValue = std::variant<int, float, std::string>;

class JsonCoordinator{
public:
    // ベースディレクトリを設定
    static void SetBaseDirectory(const std::string& baseDir){
        baseDirectory_ = baseDir;
    }

    // 項目を登録
    static void RegisterItem(const std::string& group, const std::string& key, AdjustableValue defaultValue);

    // 値を設定
    static void SetValue(const std::string& group, const std::string& key, AdjustableValue value);

    // 値を取得
    static AdjustableValue GetValue(const std::string& group, const std::string& key);

    // JSONとして保存
    static void Save(const std::string& fileName);

    // JSONからロード
    static void Load(const std::string& fileName);

private:
    // ベースディレクトリ
    static inline std::string baseDirectory_ = "resources/json/";

    // グループ名 -> (キー -> 値) のマップ
    static inline std::unordered_map<std::string, std::unordered_map<std::string, AdjustableValue>> data_;
};

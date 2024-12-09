#include "JsonCoordinator.h"

//* c++ std *//
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <iostream>

namespace nlohmann{
    template <>
    struct adl_serializer<AdjustableValue>{
        // AdjustableValueをJSON形式に変換
        static void to_json(json& j, const AdjustableValue& value){
            std::visit([&] (auto&& arg){ j = arg; }, value);
        }

        // JSONからAdjustableValueに変換
        static void from_json(const json& j, AdjustableValue& value){
            if (j.is_number_integer()){
                value = j.get<int>();
            } else if (j.is_number_float()){
                value = j.get<float>();
            } else if (j.is_string()){
                value = j.get<std::string>();
            } else{
                throw std::runtime_error("Unsupported type for AdjustableValue");
            }
        }
    };
}


// アイテムを登録する
void JsonCoordinator::RegisterItem(const std::string& group, const std::string& key, AdjustableValue defaultValue){
    data_[group][key] = defaultValue;
}

// 値を設定する
void JsonCoordinator::SetValue(const std::string& group, const std::string& key, AdjustableValue value){
    // 指定されたキーが既に登録されているか確認
    if (data_.count(group) && data_[group].count(key)){
        data_[group][key] = value;
    } else{
        throw std::runtime_error("Key not registered: " + group + "::" + key);
    }
}

// 値を取得する
AdjustableValue JsonCoordinator::GetValue(const std::string& group, const std::string& key){
    // 指定されたキーが存在するか確認
    if (data_.count(group) && data_[group].count(key)){
        return data_.at(group).at(key);
    }
    throw std::runtime_error("Key not found: " + group + "::" + key);
}

// JSONデータを保存する
void JsonCoordinator::Save(const std::string& fileName){
    // ディレクトリパスとフルパスを計算
    std::string directoryPath = baseDirectory_ + fileName.substr(0, fileName.find_last_of('.'));
    std::string fullPath = directoryPath + "/" + fileName;

    // ディレクトリ作成
    std::filesystem::create_directories(directoryPath);

    // JSONデータを保存
    json jsonData = data_;
    std::ofstream file(fullPath);
    if (!file.is_open()){
        throw std::runtime_error("Could not open file: " + fullPath);
    }
    file << jsonData.dump(4); // JSONを整形して保存

    std::cout << "Data saved successfully to: " << fullPath << std::endl;
}

// JSONデータを読み込む
void JsonCoordinator::Load(const std::string& fileName){
    // ディレクトリパスとフルパスを計算
    std::string directoryPath = baseDirectory_ + fileName.substr(0, fileName.find_last_of('.'));
    std::string fullPath = directoryPath + "/" + fileName;

    // ファイルを開いてJSONを読み込む
    try{
        std::ifstream file(fullPath);
        if (!file.is_open()){
            throw std::runtime_error("Could not open file: " + fullPath);
        }

        // JSONデータをパースして内部データ構造に復元
        json jsonData;
        file >> jsonData;
        data_ = jsonData.get<decltype(data_)>();

        std::cout << "Data loaded successfully from: " << fullPath << std::endl;
    } catch (const std::exception& e){
        // エラーが発生した場合はメッセージを出力
        std::cerr << "Error loading JSON file: " << e.what() << std::endl;
    }
}

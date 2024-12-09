#pragma once

#include <string>
#include <unordered_map>
#include <variant>
#include <nlohmann/json.hpp>


using json = nlohmann::json;
using AdjustableValue = std::variant<int, float, std::string>;

class JsonCoordinator{
public:
    // �x�[�X�f�B���N�g����ݒ�
    static void SetBaseDirectory(const std::string& baseDir){
        baseDirectory_ = baseDir;
    }

    // ���ڂ�o�^
    static void RegisterItem(const std::string& group, const std::string& key, AdjustableValue defaultValue);

    // �l��ݒ�
    static void SetValue(const std::string& group, const std::string& key, AdjustableValue value);

    // �l���擾
    static AdjustableValue GetValue(const std::string& group, const std::string& key);

    // JSON�Ƃ��ĕۑ�
    static void Save(const std::string& fileName);

    // JSON���烍�[�h
    static void Load(const std::string& fileName);

private:
    // �x�[�X�f�B���N�g��
    static inline std::string baseDirectory_ = "resources/json/";

    // �O���[�v�� -> (�L�[ -> �l) �̃}�b�v
    static inline std::unordered_map<std::string, std::unordered_map<std::string, AdjustableValue>> data_;
};

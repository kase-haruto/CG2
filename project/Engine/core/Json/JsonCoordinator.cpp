#include "JsonCoordinator.h"

//* c++ std *//
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <iostream>

namespace nlohmann{
    template <>
    struct adl_serializer<AdjustableValue>{
        // AdjustableValue��JSON�`���ɕϊ�
        static void to_json(json& j, const AdjustableValue& value){
            std::visit([&] (auto&& arg){ j = arg; }, value);
        }

        // JSON����AdjustableValue�ɕϊ�
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


// �A�C�e����o�^����
void JsonCoordinator::RegisterItem(const std::string& group, const std::string& key, AdjustableValue defaultValue){
    data_[group][key] = defaultValue;
}

// �l��ݒ肷��
void JsonCoordinator::SetValue(const std::string& group, const std::string& key, AdjustableValue value){
    // �w�肳�ꂽ�L�[�����ɓo�^����Ă��邩�m�F
    if (data_.count(group) && data_[group].count(key)){
        data_[group][key] = value;
    } else{
        throw std::runtime_error("Key not registered: " + group + "::" + key);
    }
}

// �l���擾����
AdjustableValue JsonCoordinator::GetValue(const std::string& group, const std::string& key){
    // �w�肳�ꂽ�L�[�����݂��邩�m�F
    if (data_.count(group) && data_[group].count(key)){
        return data_.at(group).at(key);
    }
    throw std::runtime_error("Key not found: " + group + "::" + key);
}

// JSON�f�[�^��ۑ�����
void JsonCoordinator::Save(const std::string& fileName){
    // �f�B���N�g���p�X�ƃt���p�X���v�Z
    std::string directoryPath = baseDirectory_ + fileName.substr(0, fileName.find_last_of('.'));
    std::string fullPath = directoryPath + "/" + fileName;

    // �f�B���N�g���쐬
    std::filesystem::create_directories(directoryPath);

    // JSON�f�[�^��ۑ�
    json jsonData = data_;
    std::ofstream file(fullPath);
    if (!file.is_open()){
        throw std::runtime_error("Could not open file: " + fullPath);
    }
    file << jsonData.dump(4); // JSON�𐮌`���ĕۑ�

    std::cout << "Data saved successfully to: " << fullPath << std::endl;
}

// JSON�f�[�^��ǂݍ���
void JsonCoordinator::Load(const std::string& fileName){
    // �f�B���N�g���p�X�ƃt���p�X���v�Z
    std::string directoryPath = baseDirectory_ + fileName.substr(0, fileName.find_last_of('.'));
    std::string fullPath = directoryPath + "/" + fileName;

    // �t�@�C�����J����JSON��ǂݍ���
    try{
        std::ifstream file(fullPath);
        if (!file.is_open()){
            throw std::runtime_error("Could not open file: " + fullPath);
        }

        // JSON�f�[�^���p�[�X���ē����f�[�^�\���ɕ���
        json jsonData;
        file >> jsonData;
        data_ = jsonData.get<decltype(data_)>();

        std::cout << "Data loaded successfully from: " << fullPath << std::endl;
    } catch (const std::exception& e){
        // �G���[�����������ꍇ�̓��b�Z�[�W���o��
        std::cerr << "Error loading JSON file: " << e.what() << std::endl;
    }
}

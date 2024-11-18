#include "Engine/objects/ModelBuilder.h"
#include "externals/imgui/imgui.h"
#include "Engine/objects/ModelManager.h"

#include <fstream>
#include <nlohmann/json.hpp> // JSONライブラリ

using json = nlohmann::json;
ModelBuilder::ModelBuilder(){}
void ModelBuilder::Initialize(){
    LoadModels("Resources/json/models.json");
}

void ModelBuilder::Update(){
    for (auto& pair : models_){
        pair.second.second->Update();
    }
}

void ModelBuilder::Draw(){
    for (auto& pair : models_){
        pair.second.second->Draw();
    }
}

void ModelBuilder::RemoveModel(size_t index){
    if (index < models_.size()){
        auto it = std::next(models_.begin(), index);
        models_.erase(it);
    }
}

void ModelBuilder::ShowImGuiInterface(){
    ImGui::Begin("Model Builder");

    // === 操作ボタン ===
    ImGui::Text("Operations:");
    ImGui::Separator();

    // モデル追加ボタン
    if (ImGui::Button("Add Model")){
        if (!selectedModelName_.empty()){
            AddModel(selectedModelName_);
        }
    }
    ImGui::SameLine();
    // モデル削除ボタン
    if (ImGui::Button("Remove Selected Model")){
        auto it = models_.find(selectedModelName_);
        if (it != models_.end()){
            models_.erase(it);
            selectedModelName_.clear();
        }
    }

    // 保存ボタン
    if (ImGui::Button("Save Models")){
        SaveModels("models.json");
    }
    ImGui::Separator();

    // === モデル選択 ===
    ImGui::Text("Select Model:");
    const auto& loadedModels = ModelManager::GetInstance()->GetLoadedModelNames();

    if (ImGui::BeginCombo("Loaded Models", selectedModelName_.c_str())){
        for (const auto& modelName : loadedModels){
            bool isSelected = (selectedModelName_ == modelName);
            if (ImGui::Selectable(modelName.c_str(), isSelected)){
                selectedModelName_ = modelName; // 選択されたモデル名を更新
            }
            if (isSelected){
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }

    ImGui::Separator();

    // === モデルリストと詳細ビュー ===
    ImGui::Columns(2, nullptr, true);

    // 左側: モデルリスト
    ImGui::Text("Model List:");
    ImGui::Separator();
    size_t index = 0;
    for (const auto& pair : models_){
        bool isSelected = (selectedModelName_ == pair.first);
        if (ImGui::Selectable(pair.first.c_str(), isSelected)){
            selectedModelName_ = pair.first; // ユニークキーを選択
        }
        index++;
    }
    ImGui::NextColumn();

    // 右側: 選択したモデルの詳細
    ImGui::Text("Model Details:");
    ImGui::Separator();
    if (!selectedModelName_.empty()){
        auto it = models_.find(selectedModelName_);
        if (it != models_.end() && it->second.second){
            ImGui::Text("Selected Model: %s", it->second.first.c_str());
            it->second.second->ShowImGuiInterface(); // 選択されたモデルの詳細UIを表示
        } else{
            ImGui::Text("Model details are unavailable.");
        }
    } else{
        ImGui::Text("No Model Selected");
    }

    ImGui::Columns(1);
    ImGui::End();
}

void ModelBuilder::AddModel(const std::string& modelName){
    static int uniqueId = 0;
    std::string uniqueKey = modelName + "_" + std::to_string(uniqueId++);
    auto modelData = ModelManager::GetInstance()->GetModelData(modelName);
    if (!modelData){
        return;
    }
    auto model = CreateModel(modelName);
    models_.emplace(uniqueKey, std::make_pair(modelName, std::move(model)));
}
std::unique_ptr<Model> ModelBuilder::CreateModel(const std::string& modelName){
    auto model = std::make_unique<Model>(modelName);
    model->SetViewProjection(viewProjection_);
    return model;
}

void ModelBuilder::SetViewProjection(ViewProjection* viewProjection){
    viewProjection_ = viewProjection;
}
/////////////////////////////////////////////////////////////////////////////////////////
//                          json
/////////////////////////////////////////////////////////////////////////////////////////
void ModelBuilder::SaveModels(const std::string& filePath){
    json jsonModels;
    for (const auto& pair : models_){
        json modelData;
        modelData["unique_key"] = pair.first;
        modelData["original_name"] = pair.second.first;
        modelData["position"] = {
            pair.second.second->transform.translate.x,
            pair.second.second->transform.translate.y,
            pair.second.second->transform.translate.z
        };
        modelData["scale"] = {
            pair.second.second->transform.scale.x,
            pair.second.second->transform.scale.y,
            pair.second.second->transform.scale.z
        };
        modelData["rotation"] = {
            pair.second.second->transform.rotate.x,
            pair.second.second->transform.rotate.y,
            pair.second.second->transform.rotate.z
        };
        jsonModels.push_back(modelData);
    }
    json outputJson;
    outputJson["models"] = jsonModels;
    std::ofstream file(filePath);
    if (file.is_open()){
        file << outputJson.dump(4);
        file.close();
    } else{
        throw std::runtime_error("Failed to open file for saving models.");
    }
}
void ModelBuilder::LoadModels(const std::string& filePath){
    std::ifstream file(filePath);
    if (!file.is_open()){
        throw std::runtime_error("Failed to open file for loading models.");
    }
    json inputJson;
    file >> inputJson;
    models_.clear();
    for (const auto& modelData : inputJson["models"]){
        std::string uniqueKey = modelData["unique_key"].get<std::string>();
        std::string originalName = modelData["original_name"].get<std::string>();

        auto model = CreateModel(originalName);

        if (modelData.contains("position")){
            auto pos = modelData["position"];
            model->transform.translate = Vector3({pos[0], pos[1], pos[2]});
        }
        if (modelData.contains("scale")){
            auto scale = modelData["scale"];
            model->transform.scale = Vector3({scale[0], scale[1], scale[2]});
        }
        if (modelData.contains("rotation")){
            auto rot = modelData["rotation"];
            model->transform.rotate = Vector3({rot[0], rot[1], rot[2]});
        }
        models_.emplace(uniqueKey, std::make_pair(originalName, std::move(model)));
    }
}
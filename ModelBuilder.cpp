#include "ModelBuilder.h"
#include"imgui.h"

ModelBuilder::~ModelBuilder(){
    models_.clear();
}

void ModelBuilder::Initialize(){}

void ModelBuilder::Update(){
	for (auto& pair : models_){
		pair.second->Update();
	}
}

void ModelBuilder::Draw(){
	for (auto& pair : models_){
		pair.second->Draw();
	}
}

void ModelBuilder::AddModel(const std::string& modelPath){
	auto model = CreateModel(modelPath+".obj");
	models_.emplace_back(modelPath + ".obj", std::move(model));
}

void ModelBuilder::RemoveModel(size_t index){
    if (index < models_.size()){
        models_.erase(models_.begin() + index);
    }
}

void ModelBuilder::ShowImGuiInterface(){
    ImGui::Begin("Model Manager");

    static char modelPath[128] = "";

    ImGui::InputText("Model Name", modelPath, IM_ARRAYSIZE(modelPath));
    if (ImGui::Button("Add Model")){
        AddModel(modelPath);
    }

    ImGui::Separator();

    for (size_t i = 0; i < models_.size(); ++i){
        ImGui::PushID(static_cast< int >(i));

        // モデルごとの小見出し
        if (ImGui::CollapsingHeader(models_[i].first.c_str())){
            // モデルごとの個別のImGui項目
            models_[i].second->ShowImGuiInterface();

            // モデルの削除ボタン
            if (ImGui::Button("Remove Model")){
                RemoveModel(i);

                ImGui::PopID();
                break;

            }
        }

        ImGui::PopID();
    }

    ImGui::End();
}

std::unique_ptr<Model> ModelBuilder::CreateModel(const std::string& modelPath){
	auto model = std::make_unique<Model>();
	model->SetViewProjection(viewProjection_);
	model->Create("Resources", modelPath);
	return model;
}



void ModelBuilder::SetViewProjection(ViewProjection* viewProjection){ viewProjection_ = viewProjection; }
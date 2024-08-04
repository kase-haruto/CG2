#include "ModelBuilder.h"
#include"imgui.h"

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

void ModelBuilder::RemoveModel(const std::string& modelPath, size_t index){
	auto it = std::remove_if(models_.begin(), models_.end(), [&] (const auto& pair){
		return pair.first == modelPath;
	});

	if (it!=models_.end()){
		models_.erase(it);
	}
}

void ModelBuilder::ShowImGuiInterface(){
	ImGui::Begin("Model Manager");

	static char modelPath[128] = "";
	ImGui::InputText("Model Path", modelPath, IM_ARRAYSIZE(modelPath));
	if (ImGui::Button("Add Model")){
		AddModel(modelPath);
	}


	for (size_t i = 0; i < models_.size(); ++i){
		ImGui::PushID(static_cast< int >(i));
		if (ImGui::Button("Remove Model")){
			RemoveModel(models_[i].first, i);
		}
		ImGui::SameLine();
		ImGui::Text("%s", models_[i].first.c_str());
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
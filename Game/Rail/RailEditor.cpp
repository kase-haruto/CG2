#include"Rail/RailEditor.h"

#ifdef _DEBUG
#include"imgui.h"
#endif // _DEBUG


RailEditor::RailEditor(){
	//モデル配列のClear
	debugModels_.clear();
}

void RailEditor::Initialize(){

}

void RailEditor::Update(){
#ifdef _DEBUG
	ImGui::Begin("RailEditor");

	ImGui::End();
#endif // _DEBUG

}

void RailEditor::Draw(){}

void RailEditor::SetViewProjection(const ViewProjection* viewProjection){
	pViewProjection_ = viewProjection;
}

void RailEditor::AddCtrlPoint(const Vector3& preCtrlPoint){
	std::unique_ptr<Model> newModel = std::make_unique<Model>("teapot");
	newModel->SetViewProjection(pViewProjection_);
	//一つ前の制御点の位置から少しずれた位置にモデルを追加
	Vector3 offset {0.0f,0.0f,1.0f};
	newModel->SetPos(preCtrlPoint + offset);

	debugModels_.emplace_back(std::move(newModel));
}

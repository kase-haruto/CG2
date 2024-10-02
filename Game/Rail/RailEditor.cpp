#include"Rail/RailEditor.h"

#include"GlobalVariable/GlobalVariables.h"

#ifdef _DEBUG
#include"imgui.h"
#endif // _DEBUG


RailEditor::RailEditor(){
	ctrlPoints_.clear();
	ctrlPoints_.resize(4);
	const char* groupName = Rail;
	GlobalVariables::GetInstance()->CreateGroup(groupName);
	for (size_t i = 0; i < ctrlPoints_.size(); i++){
		GlobalVariables::GetInstance()->AddItem(groupName, "ctrlPoint[" + i + "]", ctrlPoints_[i]);
	}

	//モデル配列のClear
	debugModels_.clear();
}

void RailEditor::Initialize(){
	//仮のポイントをうつ
	ctrlPoints_ = {
	{0,0,0},
	{0,0,10},
	{0,0,20},
	{0,0,30},
	};
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
	//デバッグ用モデルの配列を増やす
	debugModels_.emplace_back(std::move(newModel));

	//制御点を増やす
	ctrlPoints_.emplace_back()

	GlobalVariables::GetInstance()->AddItem(groupName, "ctrlPoint[" + ctrlPoints_.size() + "]", ctrlPoints_[ctrlPoints_.size()]);
}

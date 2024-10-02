#include"Rail/RailEditor.h"

#include"GlobalVariable/GlobalVariables.h"

#ifdef _DEBUG
#include"imgui.h"
#endif // _DEBUG
#include<iostream>
#include<fstream>


RailEditor::RailEditor(){
}


void RailEditor::Initialize(){
	//仮のポイントをうつ
	ctrlPoints_ = {
	{0,0,0},
	{0,0,10},
	{0,0,20},
	{0,0,30},
	};

	LoadControlPointFromJson();

}

void RailEditor::Update(){
#ifdef _DEBUG
	ImGui::Begin("RailEditor");

	// Saveボタン
	if (ImGui::Button("Save")){
		SaveControlPointToJson(); // ボタンが押されたときにセーブ
	}

	// Add Control Pointボタン
	if (ImGui::Button("Add Control Point")){
		// 最後の制御点から新しい制御点を追加
		if (!ctrlPoints_.empty()){
			AddCtrlPoint(ctrlPoints_.back());
		}
	}

	// 制御点リストを表示
	ImGui::Text("Control Points:");
	for (int i = 0; i < ctrlPoints_.size(); ++i){
		std::string label = "Point[" + std::to_string(i) + "]";
		// ラジオボタンで制御点を選択できるようにする
		if (ImGui::RadioButton(label.c_str(), selectedCtrlPoint_ == i)){
			selectedCtrlPoint_ = i; // 選択された制御点のインデックスを更新
		}
		ImGui::DragFloat3(label.c_str(), &ctrlPoints_[i].x, 0.01f);

	}

	// Delete Control Pointボタン
	if (ImGui::Button("Delete Control Point") && selectedCtrlPoint_ >= 0){
		// 選択された制御点を削除
		ctrlPoints_.erase(ctrlPoints_.begin() + selectedCtrlPoint_);
		debugModels_.erase(debugModels_.begin() + selectedCtrlPoint_);

		// インデックスをリセット
		selectedCtrlPoint_ = -1;
	}

	ImGui::End();
#endif // _DEBUG

	// デバッグ用モデルの座標を更新
	for (size_t i = 0; i < ctrlPoints_.size(); i++){
		debugModels_[i]->SetPos(ctrlPoints_[i]);
		debugModels_[i]->Update();
	}
}


void RailEditor::Draw(){
	for (auto& model : debugModels_){
		model->Draw();
	}
}

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
	ctrlPoints_.emplace_back();
}

void RailEditor::SaveControlPointToJson(){
	nlohmann::json json;

	// 制御点の配列をJSON形式に変換
	json["control_points"] = nlohmann::json::array();
	for (const auto& point : ctrlPoints_){
		json["control_points"].push_back({{"x", point.x}, {"y", point.y}, {"z", point.z}});
	}

	// Resources/json フォルダに保存
	std::filesystem::path dir("Resources/json/railControlPoint");
	if (!std::filesystem::exists(dir)){
		std::filesystem::create_directories(dir);
	}

	// 固定のファイル名で保存
	std::filesystem::path savePath = dir / "railControl_points.json";
	std::ofstream file(savePath);

	if (file.is_open()){
		file << json.dump(4); // インデント4で整形して保存
		file.close();
	} else{
		std::cerr << "Failed to open file for writing: " << savePath << std::endl;
	}
}


void RailEditor::LoadControlPointFromJson(){
	nlohmann::json json;

	// Resources/json フォルダから読み込み
	std::filesystem::path loadPath = "Resources/json/railControlPoint/railControl_points.json";
	std::ifstream file(loadPath);

	if (file.is_open()){
		file >> json;
		file.close();
	} else{
		std::cerr << "Failed to open file for reading: " << loadPath << std::endl;
		return;
	}

	// 制御点の配列をクリアしてから読み込む
	ctrlPoints_.clear();
	debugModels_.clear(); // デバッグモデルもクリア

	for (const auto& point : json["control_points"]){
		Vector3 ctrlPoint;
		ctrlPoint.x = point["x"].get<float>();
		ctrlPoint.y = point["y"].get<float>();
		ctrlPoint.z = point["z"].get<float>();
		ctrlPoints_.push_back(ctrlPoint);

		// 各制御点に対応するモデルを生成し、位置を設定
		std::unique_ptr<Model> model = std::make_unique<Model>("teapot");
		model->SetViewProjection(pViewProjection_);
		model->SetPos(ctrlPoint);
		debugModels_.emplace_back(std::move(model));
	}
}

#include "Rail/RailEditor.h"
#include "GlobalVariable/GlobalVariables.h"
#include "myFunc/PrimitiveDrawer.h"
#include "myFunc/MathFunc.h"

#ifdef _DEBUG
#include "imgui.h"
#endif // _DEBUG
#include <iostream>
#include <fstream>

RailEditor::RailEditor(){}

RailEditor::~RailEditor(){}

void RailEditor::Initialize(){
    // 仮の制御点を設定
    ctrlPoints_ = {
        {0, 0, 0},
        {0, 0, 10},
        {0, 0, 20},
        {0, 0, 30},
    };

    // 制御点をJSONから読み込み
    LoadControlPointFromJson();

    // レールモデルを分割数に合わせて初期化
    const size_t segmentCount = 100;
    railModels_.reserve(segmentCount + 1); // 必要なモデル数を事前に確保

    for (size_t i = 0; i <= segmentCount; ++i){
        auto model = std::make_unique<Model>("rail");
        model->SetViewProjection(pViewProjection_);
        railModels_.emplace_back(std::move(model));
    }
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
        // 新しい制御点を追加する位置を決定
        if (!ctrlPoints_.empty()){
            if (selectedCtrlPoint_ >= 0 && selectedCtrlPoint_ < ctrlPoints_.size()){
                // 選択された制御点の後ろに追加
                AddCtrlPoint(ctrlPoints_[selectedCtrlPoint_], selectedCtrlPoint_ + 1);
            } else{
                // 選択されていない場合は最後に追加
                AddCtrlPoint(ctrlPoints_.back(), ctrlPoints_.size());
            }
        }
    }

    // Delete Control Pointボタン
    if (ImGui::Button("Delete Control Point") && selectedCtrlPoint_ >= 0){
        // 選択された制御点を削除
        if (selectedCtrlPoint_ < ctrlPoints_.size()){
            ctrlPoints_.erase(ctrlPoints_.begin() + selectedCtrlPoint_);
            debugModels_.erase(debugModels_.begin() + selectedCtrlPoint_);

            // インデックスをリセット
            selectedCtrlPoint_ = -1;
        }
    }

    // 制御点リストを表示
    ImGui::Text("Control Points:");
    ImGui::BeginChild("ControlPointsList", ImVec2(0, 200), true, ImGuiWindowFlags_AlwaysVerticalScrollbar);

    for (int i = 0; i < ctrlPoints_.size(); ++i){
        std::string label = "Point[" + std::to_string(i) + "]";
        bool isSelected = (selectedCtrlPoint_ == i);

        // ラジオボタンで制御点を選択・解除できるようにする
        if (ImGui::RadioButton(label.c_str(), isSelected)){
            if (isSelected){
                selectedCtrlPoint_ = -1; // 選択解除
            } else{
                selectedCtrlPoint_ = i; // 選択
            }
        }
        ImGui::DragFloat3(label.c_str(), &ctrlPoints_[i].x, 0.01f);
    }

    ImGui::EndChild();
    ImGui::End();
#endif // _DEBUG

    // デバッグ用モデルの座標を更新
    for (size_t i = 0; i < ctrlPoints_.size(); i++){
        if (i == selectedCtrlPoint_){
            debugModels_[i]->SetColor(Vector4(1.0f, 0.0f, 0.0f, 1.0f)); // 選択されたモデルは赤
        } else{
            debugModels_[i]->SetColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f)); // それ以外は白
        }

        debugModels_[i]->SetPos(ctrlPoints_[i]);
        debugModels_[i]->Update();
    }
}

void RailEditor::Draw(){
    DrawLine();
    // レールモデルの描画
    for (auto& model : railModels_){
        model->Draw();
    }
}

// upベクトルを指定された軸と角度で回転させる関数
Vector3 RotateVectorAroundAxis( Vector3 vector, const Vector3& axis, float angle){
    float cosAngle = cos(angle);
    float sinAngle = sin(angle);
    return vector * cosAngle + Cross(axis, vector) * sinAngle + axis * vector.Dot(axis) * (1.0f - cosAngle);
}

void RailEditor::DrawLine(){
    std::vector<Vector3> pointsDrawing;
    const size_t segmentCount = 100;

    // Catmull-Romスプラインの分割点を計算
    for (size_t i = 0; i <= segmentCount; i++){
        float t = 1.0f / segmentCount * i;
        Vector3 pos = CatmullRomPosition(ctrlPoints_, t);
        pointsDrawing.push_back(pos);
    }

    // 初期のforwardとupベクトルを設定
    Vector3 forward = (pointsDrawing[1] - pointsDrawing[0]).Normalize();
    Vector3 up = {0, 1, 0}; // 初期の上方向を設定

    // 各分割点に既存のモデルを配置
    for (size_t i = 0; i <= segmentCount; ++i){
        railModels_[i]->SetPos(pointsDrawing[i]);

        // 次の方向ベクトルを計算（最後のセグメントの場合は直前を使用）
        Vector3 nextForward;
        if (i < segmentCount){
            nextForward = (pointsDrawing[i + 1] - pointsDrawing[i]).Normalize();
        } else{
            nextForward = forward;
        }

        // 回転軸と角度を求めて平行輸送
        Vector3 rotationAxis = Cross(forward, nextForward);
        float angle = acos(forward.Dot(nextForward));

        if (rotationAxis.Length() > 0.0001f){
            up = RotateVectorAroundAxis(up, rotationAxis.Normalize(), angle);
        }

        // 基底ベクトルを用いて回転行列を作成
        Vector3 right = Cross(up, nextForward).Normalize();
        Matrix4x4 rotationMatrix = Matrix4x4::FromBasis(right, up, nextForward);
        Vector3 rotation = Matrix4x4::MatrixToEuler(rotationMatrix);
        railModels_[i]->transform.rotate = rotation;

        railModels_[i]->Update();
        forward = nextForward;
    }

    // ラインを描画
    for (size_t i = 0; i < segmentCount; i++){
        PrimitiveDrawer::GetInstance()->DrawLine3d(pointsDrawing[i], pointsDrawing[i + 1], {1.0f, 1.0f, 1.0f, 1.0f});
    }
}






void RailEditor::SetViewProjection(const ViewProjection* viewProjection){
    pViewProjection_ = viewProjection;
}

void RailEditor::AddCtrlPoint(const Vector3& preCtrlPoint, size_t insertIndex){
    // 新しいモデルを生成
    auto newModel = std::make_unique<Model>("debugCube");
    newModel->SetViewProjection(pViewProjection_);
    Vector3 offset {0.0f, 0.0f, 1.0f};
    Vector3 newCtrlPoint = preCtrlPoint + offset;

    // 指定された位置に制御点とモデルを挿入
    ctrlPoints_.insert(ctrlPoints_.begin() + insertIndex, newCtrlPoint);
    newModel->SetPos(newCtrlPoint);
    debugModels_.insert(debugModels_.begin() + insertIndex, std::move(newModel));
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
        auto model = std::make_unique<Model>("debugCube");
        model->SetViewProjection(pViewProjection_);
        model->SetPos(ctrlPoint);
        debugModels_.emplace_back(std::move(model));
    }
}

void RailEditor::AddNewRailModel(const Vector3& spawnPos){
    // 事前に初期化済みのモデルを利用するため、この関数は不要になる可能性があります。
}

#include "ParticleSystem.h"

#include "ParticleManager.h"
#include "lib/myFunc/Random.h"

// lib
#include <externals/imgui/imgui.h>
#include <externals/nlohmann/json.hpp>
#include <fstream>
#include <algorithm>
using json = nlohmann::json;


ParticleSystem::ParticleSystem(){}

void ParticleSystem::Initialize(const std::string& modelName, const std::string& texturePath){
    Load(fileDirectoryPath + GetName() + ".json");

    BaseParticle::Initialize(modelName,texturePath, emitter_.count);

    ParticleManager::GetInstance()->AddSystem(this);

}


//===================================================================*/
//                    json/ui
//===================================================================*/
void ParticleSystem::ImGui(){
    if (ImGui::Button("Save to JSON")){
        Save(fileDirectoryPath + GetName() + ".json");
    }

    // Particle Colorセクション
    if (ImGui::TreeNode("Particle Color")){
        ImGui::Checkbox("fixationAlpha" ,&isFixationAlpha_);
        const char* modes[] = {"Random", "Single Color", "Similar Color"};
        int currentMode = static_cast< int >(colorMode_);
        if (ImGui::Combo("Color Mode", &currentMode, modes, IM_ARRAYSIZE(modes))){
            colorMode_ = static_cast< ColorMode >(currentMode);
        }

        if (colorMode_ == ColorMode::SingleColor){
            ImGui::ColorEdit4("Particle Color", &selectedColor_.x, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
            ImGui::SameLine();
            ImGui::Text("Particle Color");
        } else if (colorMode_ == ColorMode::SimilarColor){
            ImGui::ColorEdit4("Base Color", &selectedColor_.x, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
            ImGui::SameLine();
            ImGui::Text("Base Color");

            ImGui::DragFloat("Color Variation", &colorVariation_, 0.01f, 0.0f, 1.0f);
        }

        ImGui::TreePop(); // 折りたたみ終了
    }

    // Emitterセクション
    if (ImGui::TreeNode("Emitter")){
        ImGui::Checkbox("isBillboard", &isBillboard_);
        ImGui::DragFloat3("emitter:pos", &emitter_.transform.translate.x, 0.01f);
        ImGui::DragFloat3("emitter:rotate", &emitter_.transform.rotate.x, 0.01f);
        ImGui::DragFloat("emitter:frequencyTime", &emitter_.frequency, 0.1f);
        int count = emitter_.count;
        ImGui::DragInt("emitter:count", &count, 1);
        emitter_.count = count;
        BaseParticle::ImGui();

        ImGui::TreePop();
    }

    // Behaviorセクション
    if (ImGui::TreeNode("Behavior")){
        if (behavior_){
            behavior_->ShowImGui();
        }
        ImGui::TreePop();
    }
}


void ParticleSystem::Save(const std::string& filename){

    json j;

    // 基本情報
    j["name"] = name_;

    // カラーモードとカラー関連
    j["colorMode"] = static_cast< int >(colorMode_);

    std::vector<float> colorVec = {selectedColor_.x, selectedColor_.y, selectedColor_.z, selectedColor_.w};
    j["selectedColor"] = colorVec;
    j["colorVariation"] = colorVariation_;

    // emitter
    {
        json e;
        e["count"] = emitter_.count;
        e["frequency"] = emitter_.frequency;
        e["frequencyTime"] = emitter_.frequencyTime;

        // Transform
        {
            json t;
            t["translate"] = {emitter_.transform.translate.x, emitter_.transform.translate.y, emitter_.transform.translate.z};
            t["rotate"] = {emitter_.transform.rotate.x, emitter_.transform.rotate.y, emitter_.transform.rotate.z};
            t["scale"] = {emitter_.transform.scale.x, emitter_.transform.scale.y, emitter_.transform.scale.z};
            e["transform"] = t;
        }

        j["emitter"] = e;
    }

    // 発生面フラグ
    {
        json faces;
        faces["emitPosX"] = emitPosX_;
        faces["emitNegX"] = emitNegX_;
        faces["emitPosY"] = emitPosY_;
        faces["emitNegY"] = emitNegY_;
        faces["emitPosZ"] = emitPosZ_;
        faces["emitNegZ"] = emitNegZ_;
        j["faces"] = faces;
    }

    behavior_->SaveJson(j);

    // ファイルへ書き出し
    std::ofstream ofs(filename);
    if (ofs){
        ofs << j.dump(4);
    }
}

void ParticleSystem::Load(const std::string& filename){

    std::ifstream ifs(filename);
    if (!ifs){
        // ファイルがない場合などは何もしない
        return;
    }

    json j;
    ifs >> j;

    // 基本情報
    if (j.contains("name")){
        name_ = j["name"].get<std::string>();
    }

    // カラーモードとカラー関連
    if (j.contains("colorMode")){
        colorMode_ = static_cast< ColorMode >(j["colorMode"].get<int>());
    }

    if (j.contains("selectedColor")){
        auto colorVec = j["selectedColor"].get<std::vector<float>>();
        if (colorVec.size() == 4){
            selectedColor_.x = colorVec[0];
            selectedColor_.y = colorVec[1];
            selectedColor_.z = colorVec[2];
            selectedColor_.w = colorVec[3];
        }
    }

    if (j.contains("colorVariation")){
        colorVariation_ = j["colorVariation"].get<float>();
    }

    // emitter
    if (j.contains("emitter")){
        auto e = j["emitter"];
        if (e.contains("count")){
            emitter_.count = e["count"].get<uint32_t>();
        }
        if (e.contains("frequency")){
            emitter_.frequency = e["frequency"].get<float>();
        }
        if (e.contains("frequencyTime")){
            emitter_.frequencyTime = e["frequencyTime"].get<float>();
        }

        // Transform
        if (e.contains("transform")){
            auto t = e["transform"];
            if (t.contains("translate")){
                auto tr = t["translate"].get<std::vector<float>>();
                if (tr.size() == 3){
                    emitter_.transform.translate = {tr[0], tr[1], tr[2]};
                }
            }
            if (t.contains("rotate")){
                auto rr = t["rotate"].get<std::vector<float>>();
                if (rr.size() == 3){
                    emitter_.transform.rotate = {rr[0], rr[1], rr[2]};
                }
            }
            if (t.contains("scale")){
                auto sc = t["scale"].get<std::vector<float>>();
                if (sc.size() == 3){
                    emitter_.transform.scale = {sc[0], sc[1], sc[2]};
                }
            }
        }
    }

    // 発生面フラグの読み込み
    if (j.contains("faces")){
        auto f = j["faces"];
        if (f.contains("emitPosX")) emitPosX_ = f["emitPosX"].get<bool>();
        if (f.contains("emitNegX")) emitNegX_ = f["emitNegX"].get<bool>();
        if (f.contains("emitPosY")) emitPosY_ = f["emitPosY"].get<bool>();
        if (f.contains("emitNegY")) emitNegY_ = f["emitNegY"].get<bool>();
        if (f.contains("emitPosZ")) emitPosZ_ = f["emitPosZ"].get<bool>();
        if (f.contains("emitNegZ")) emitNegZ_ = f["emitNegZ"].get<bool>();
    }

    behavior_->LoadJson(j);

}


bool ParticleSystem::GetUseRandomColor() const{
    // ランダムモードのときのみtrueを返す
    return (colorMode_ == ColorMode::Random);
}

Vector4 ParticleSystem::GetSelectedColor() const{
    if (colorMode_ == ColorMode::SingleColor){
        // 単色モードは選択色をそのまま返す
        return selectedColor_;
    } else if (colorMode_ == ColorMode::SimilarColor){
        // 類似色モードの場合、呼ばれるたびにバラつきを反映した色を返す
        // ここで乱数を使うと、フレームごとに色が変わってしまうため、
        // Emit()内で色を決めるように変更することも検討してください。
        // しかし質問文の状況を踏まえ、このままでも機能はします。
        Vector4 c = selectedColor_;
        c.x = std::clamp(c.x + Random::Generate(-colorVariation_, colorVariation_), 0.0f, 1.0f);
        c.y = std::clamp(c.y + Random::Generate(-colorVariation_, colorVariation_), 0.0f, 1.0f);
        c.z = std::clamp(c.z + Random::Generate(-colorVariation_, colorVariation_), 0.0f, 1.0f);
        return c;
    }

    // デフォルトはランダムでも単色でもない場合用
    return Vector4(1.0f, 1.0f, 1.0f, 1.0f);
}

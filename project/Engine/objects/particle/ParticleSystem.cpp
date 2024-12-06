#include "ParticleSystem.h"

#include "ParticleManager.h"

// lib
#include <externals/imgui/imgui.h>
#include <externals/nlohmann/json.hpp>
#include <fstream>
using json = nlohmann::json;


ParticleSystem::ParticleSystem(){}

void ParticleSystem::Initialize(const std::string& modelName, const std::string& texturePath){

    BaseParticle::Initialize(modelName,texturePath, emitter_.count);

    ParticleManager::GetInstance()->AddSystem(this);

    Load(fileDirectoryPath + GetName() + ".json");
}


//===================================================================*/
//                    json/ui
//===================================================================*/
void ParticleSystem::ImGui(){
    // 保存用のボタンを配置
    if (ImGui::Button("Save to JSON")){
        Save( fileDirectoryPath+ GetName() + ".json");
    }

    ImGui::Text("transform");
    ImGui::DragFloat3("emitter:pos", &emitter_.transform.translate.x, 0.01f);
    ImGui::DragFloat3("emitter:rotate", &emitter_.transform.rotate.x, 0.01f);

    ImGui::Separator();

    ImGui::DragFloat("emitter:frequencyTime", &emitter_.frequency, 0.1f);
    int count = emitter_.count;
    ImGui::DragInt("emitter:count", &count, 1);
    emitter_.count = count;

    // ランダムカラー使用切り替え
    ImGui::Checkbox("Use Random Color", &useRandomColor_);
    if (!useRandomColor_){
        // ランダムカラーOFFなら色を選択
        ImGui::ColorPicker4("Particle Color", &selectedColor_.x);
    }

    ImGui::Separator();
    ImGui::Text("emitter");
    BaseParticle::ImGui();

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
    if (j.contains("useRandomColor")){
        useRandomColor_ = j["useRandomColor"].get<bool>();
    }

    // 色
    if (j.contains("selectedColor")){
        auto colorVec = j["selectedColor"].get<std::vector<float>>();
        if (colorVec.size() == 4){
            selectedColor_.x = colorVec[0];
            selectedColor_.y = colorVec[1];
            selectedColor_.z = colorVec[2];
            selectedColor_.w = colorVec[3];
        }
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

}

void ParticleSystem::Save(const std::string& filename){

    json j;

    // 基本情報
    j["name"] = name_;
    j["useRandomColor"] = useRandomColor_;

    // 色
    {
        std::vector<float> colorVec = {selectedColor_.x, selectedColor_.y, selectedColor_.z, selectedColor_.w};
        j["selectedColor"] = colorVec;
    }

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

    // ファイルへ書き出し
    std::ofstream ofs(filename);
    if (ofs){
        ofs << j.dump(4); // インデント付きで保存
    }

}

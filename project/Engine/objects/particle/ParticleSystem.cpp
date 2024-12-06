#include "ParticleSystem.h"

#include "ParticleManager.h"

// lib
#include <externals/imgui/imgui.h>

ParticleSystem::ParticleSystem(){}

void ParticleSystem::Initialize(const std::string& modelName, const std::string& texturePath){

    BaseParticle::Initialize(modelName,texturePath, emitter_.count);

    ParticleManager::GetInstance()->AddSystem(this);

}

void ParticleSystem::ImGui(){
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
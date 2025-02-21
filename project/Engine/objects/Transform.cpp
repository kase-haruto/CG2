#include "Transform.h"

#include <externals/imgui/imgui.h>

void EulerTransform::ShowImGui(const std::string& lavel){
    if (ImGui::CollapsingHeader(lavel.c_str())){
        ImGui::DragFloat3("scale", &scale.x, 0.01f);
        ImGui::DragFloat3("rotation", &rotate.x, 0.01f);
        ImGui::DragFloat3("translate", &translate.x, 0.01f);
    }
}

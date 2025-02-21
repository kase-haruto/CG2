#include "Material.h"

#include <externals/imgui/imgui.h>

void Material::ShowImGui(){
	ImGui::DragFloat("shininess", &shininess, 0.01f);

	const char* lightingModes[] = {"Half-Lambert", "Lambert", "SpecularReflection", "No Lighting"};
	if (ImGui::BeginCombo("Lighting Mode", lightingModes[currentLightingMode_])){
		for (int n = 0; n < IM_ARRAYSIZE(lightingModes); n++){
			bool is_selected = (currentLightingMode_ == n);
			if (ImGui::Selectable(lightingModes[n], is_selected)){
				currentLightingMode_ = n;
				enableLighting = currentLightingMode_;
			}
			if (is_selected){
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}
}

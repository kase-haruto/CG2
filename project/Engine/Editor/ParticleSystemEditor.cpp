#include "ParticleSystemEditor.h"

#include <Engine/System/Command/EditorCommand/GuiCommand/ImGuiHelper/GuiCmd.h>
#include <Engine/Foundation/Clock/ClockManager.h>

#include <externals/imgui/imgui.h>

/////////////////////////////////////////////////////////////////////////////////////////
//		gui
/////////////////////////////////////////////////////////////////////////////////////////
void ParticleSystemEditor::ShowImGuiInterface(){
	FxEmitter* emitter = previewEmitter_.get();
	if (!emitter) return;

	ImGui::Text("Emitter Settings");
	GuiCmd::DragFloat("Emit Rate", emitter->emitRate_, 0.01f, 0.01f, 10.0f);
	GuiCmd::DragFloat3("Position", emitter->position_);

	if (ImGui::CollapsingHeader("Velocity Module")){
		int mode = static_cast< int >(emitter->velocity_.GetMode());
		if (ImGui::Combo("Mode", &mode, "Constant\0Random\0")){
			if (mode == 0) emitter->velocity_.SetConstant(emitter->velocity_.Get());
			else emitter->velocity_.SetRandom(Vector3(-1, 0, -1), Vector3(1, 0, 1));
		}
		if (mode == 0){
			Vector3 val = emitter->velocity_.GetConstant();
			if (GuiCmd::DragFloat3("Constant Velocity", val)){
				emitter->velocity_.SetConstant(val);
			}
		} else{
			Vector3 min = emitter->velocity_.GetMin();
			Vector3 max = emitter->velocity_.GetMax();
			if (GuiCmd::DragFloat3("Min Velocity", min)){
				emitter->velocity_.SetRandom(min, emitter->velocity_.GetMax());
			}
			if (GuiCmd::DragFloat3("Max Velocity", max)){
				emitter->velocity_.SetRandom(emitter->velocity_.GetMin(), max);
			}
		}
	}

	if (ImGui::CollapsingHeader("Lifetime Module")){
		int mode = static_cast< int >(emitter->lifetime_.GetMode());
		if (ImGui::Combo("Mode##Life", &mode, "Constant\0Random\0")){
			if (mode == 0) emitter->lifetime_.SetConstant(emitter->lifetime_.Get());
			else emitter->lifetime_.SetRandom(1.0f, 3.0f);
		}
		if (mode == 0){
			float val = emitter->lifetime_.GetConstant();
			if (GuiCmd::DragFloat("Constant Lifetime", val, 0.1f)){
				emitter->lifetime_.SetConstant(val);
			}
		} else{
			float min = emitter->lifetime_.GetMin();
			float max = emitter->lifetime_.GetMax();
			if (GuiCmd::DragFloat("Min Lifetime", min, 0.1f)){
				emitter->lifetime_.SetRandom(min, emitter->lifetime_.GetMax());
			}
			if (GuiCmd::DragFloat("Max Lifetime", max, 0.1f)){
				emitter->lifetime_.SetRandom(emitter->lifetime_.GetMin(), max);
			}
		}
	}

	if (ImGui::CollapsingHeader("Size")){
		float size = emitter->defaultSize_;
		if (GuiCmd::DragFloat("Size##Emitter", size, 0.1f, 0.01f, 100.0f)){
			emitter->defaultSize_ = size;
		}
	}
}


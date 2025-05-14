#include "ParticleEditPanel.h"
#include <externals/imgui/imgui.h>

ParticleEditPanel::ParticleEditPanel()
	:IEngineUI("ParticleEdit"){}

void ParticleEditPanel::Render(){
	particleManager_->ShowDebugUI();
}

const std::string& ParticleEditPanel::GetPanelName() const{
	return panelName_;
}
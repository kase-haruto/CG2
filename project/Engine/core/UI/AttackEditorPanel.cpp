#include "AttackEditorPanel.h"
#include <externals/imgui/imgui.h>

AttackEditorPanel::AttackEditorPanel()
:IEngineUI("Editor"){
	isShow_ = true;
}

void AttackEditorPanel::Render(){
	playerAttackEditor_->ShowGui();
}

const std::string& AttackEditorPanel::GetPanelName() const{
	return panelName_;
}


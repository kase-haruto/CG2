#include "ConsolePanel.h"
#include <externals/imgui/imgui.h>
#include "Engine/Collision/CollisionManager.h"

ConsolePanel::ConsolePanel() 
	: IEngineUI("Console"){}

void ConsolePanel::Render(){
	ImGui::Begin(panelName_.c_str());

	const char* logTypes[] = {"System Logs","Collision Logs"};

	if (ImGui::Combo("select log", &selectedLogType_, logTypes, IM_ARRAYSIZE(logTypes))){}
	if (selectedLogType_ == 0){
		
	}

	if (selectedLogType_ == 1){
		CollisionManager::GetInstance()->DebugLog();
	}

	ImGui::End();
}

const std::string& ConsolePanel::GetPanelName() const{
	return panelName_;
}

void ConsolePanel::AddLog([[maybe_unused]] const std::string& log){}

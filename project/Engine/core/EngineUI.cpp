#include "EngineUI.h"

#include <externals/imgui/imgui.h>

void EngineUI::Initialize(){

}

void EngineUI::Render(){
#ifdef _DEBUG
    RenderDockSpace();
    RenderToolbar();
    RenderPanels();
    RenderMainViewport();
    RenderFloatingWindows();
#endif // _DEBUG

    
}

void EngineUI::SetMainViewportCallback(std::function<void()> callback){
    mainViewportCallback_ = callback;
}

void EngineUI::SetToolbarCallback(std::function<void()> callback){
    toolbarCallback_ = callback;
}

void EngineUI::AddPanelCallback(const std::string& panelName, std::function<void()> callback){
    panelCallbacks_.emplace_back(panelName, callback);
}

void EngineUI::SetFloatingWindowCallback(std::function<void()> callback){
    floatingWindowCallback_ = callback;
}

void EngineUI::RenderDockSpace(){
    ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
}

void EngineUI::RenderToolbar(){
    if (toolbarCallback_){
        if (ImGui::Begin("Toolbar", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize)){
            toolbarCallback_();
        }
        ImGui::End();
    }
}

void EngineUI::RenderPanels(){
    for (const auto& panel : panelCallbacks_){
        if (ImGui::Begin(panel.first.c_str())){
            panel.second();
        }
        ImGui::End();
    }
}

void EngineUI::RenderMainViewport(){
    if (mainViewportCallback_){
        if (ImGui::Begin("Viewport", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse)){
            mainViewportCallback_();
        }
        ImGui::End();
    }
}

void EngineUI::RenderFloatingWindows(){
    if (floatingWindowCallback_){
        if (ImGui::Begin("Settings")){
            floatingWindowCallback_();
        }
        ImGui::End();
    }
}

#include "EngineUI.h"

#include <externals/imgui/imgui.h>

EngineUI* EngineUI::pInstance_ = nullptr;

EngineUI* EngineUI::GetInstance(){
    //　インスタンスがなければ作成
    if (!pInstance_){
        pInstance_ = new EngineUI();
    }
	return pInstance_;
}

void EngineUI::Initialize(){

    //インスタンスの生成
    GetInstance();

}

void EngineUI::Render(){
#ifdef _DEBUG
    pInstance_->RenderDockSpace();
    pInstance_->RenderToolbar();
    pInstance_->RenderPanels();
    pInstance_->RenderMainViewport();
    pInstance_->RenderFloatingWindows();
#endif // _DEBUG

    
}

void EngineUI::Finalize(){
	delete pInstance_;
	pInstance_ = nullptr;
}

void EngineUI::SetMainViewportCallback(std::function<void()> callback){
    pInstance_->mainViewportCallback_ = callback;
}

void EngineUI::SetToolbarCallback(std::function<void()> callback){
    pInstance_->toolbarCallback_ = callback;
}

void EngineUI::AddPanelCallback(const std::string& panelName, std::function<void()> callback){
    pInstance_->panelCallbacks_.emplace_back(panelName, callback);
}

void EngineUI::SetFloatingWindowCallback(std::function<void()> callback){
    pInstance_->floatingWindowCallback_ = callback;
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

#include "EngineUI.h"
#include "UI/HierarchyPanel.h"
#include "UI/InspectorPanel.h"
#include "UI/ConsolePanel.h"
#include <externals/imgui/imgui.h>

//===================================================================*/
//                   静的変数の初期化
//===================================================================*/
EngineUI* EngineUI::pInstance_ = nullptr;

//===================================================================*/
//                   シングルトンインスタンス取得
//===================================================================*/
EngineUI* EngineUI::GetInstance(){
    if (!pInstance_){
        pInstance_ = new EngineUI();
    }
    return pInstance_;
}

//===================================================================*/
//                   初期化
//===================================================================*/
void EngineUI::Initialize(){
    GetInstance();

    // パネルの追加
    pInstance_->panels_.emplace_back(std::make_unique<HierarchyPanel>());
    pInstance_->panels_.emplace_back(std::make_unique<InspectorPanel>());
    pInstance_->panels_.emplace_back(std::make_unique<ConsolePanel>());
}

//===================================================================*/
//                   レンダリング
//===================================================================*/
void EngineUI::Render(){
    ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
    for (const auto& panel : GetInstance()->panels_){
        panel->Render(); // 各パネルを描画
    }
    pInstance_->RenderMainViewport();
}


void EngineUI::RenderMainViewport(){
    ImGui::Begin("Main Viewport");

    if (mainViewportTextureID_){
        ImVec2 viewportSize = ImGui::GetContentRegionAvail();
        ImGui::Image(reinterpret_cast< ImTextureID >(mainViewportTextureID_), viewportSize);
    } else{
        ImGui::Text("Viewport texture not set.");
    }

    ImGui::End();
}


//===================================================================*/
//                   終了処理
//===================================================================*/
void EngineUI::Finalize(){
    delete pInstance_;
    pInstance_ = nullptr;
}

//===================================================================*/
//                   パネル追加
//===================================================================*/
void EngineUI::AddPanel(std::unique_ptr<IEngineUI> panel){
    panels_.push_back(std::move(panel));
}

void EngineUI::SetMainViewportTexture(UINT64 textureID){
    if (pInstance_->mainViewportTextureID_){ return; }
    pInstance_->mainViewportTextureID_ = textureID;
}
#pragma once

#include "IEngineUI.h"
#include "Engine/objects/SceneObject.h"
#include <string>

class InspectorPanel 
    : public IEngineUI{
public:
    //===================================================================*/
    //                   public functions
    //===================================================================*/
	InspectorPanel();
    ~InspectorPanel() override = default;

    void Render() override; // 描画処理
    const std::string& GetPanelName() const override; // パネル名の取得

private:
    //===================================================================*/
    //                   private variables
    //===================================================================*/
};

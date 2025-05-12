#pragma once
/* ========================================================================
/*      include space
/* ===================================================================== */
// engine
#include <Engine/Application/UI/EngineUI/IEngineUI.h>
#include <Engine/objects/SceneObject.h>

// c++
#include <string>

//forward declaration
class EditorPanel;

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

	void SetEditorPanel(EditorPanel* pEditorPanel){ pEditorPanel_ = pEditorPanel; }

private:
    //===================================================================*/
    //                   private variables
    //===================================================================*/
	EditorPanel* pEditorPanel_; // エディタパネル
};

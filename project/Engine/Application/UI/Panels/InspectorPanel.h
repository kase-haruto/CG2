#pragma once
/* ========================================================================
/*      include space
/* ===================================================================== */
// engine
#include <Engine/Application/UI/EngineUI/IEngineUI.h>
#include <Engine/Objects/3D/Actor/SceneObject.h>
#include <Engine/Application/UI/EngineUI/Context/EditorContext.h>
#include <Engine/Editor/SceneObjectEditor.h>

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
	void SetEditorContext(class EditorContext* context) { pEditorContext_ = context; }
private:
    //===================================================================*/
    //                   private variables
    //===================================================================*/
	std::unique_ptr<SceneObjectEditor> pSceneObjectEditor_; // シーンオブジェクトエディタ
	EditorPanel* pEditorPanel_; // エディタパネル
	EditorContext* pEditorContext_; // context ポインタ
};

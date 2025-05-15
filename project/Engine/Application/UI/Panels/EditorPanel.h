#pragma once
/* ========================================================================
/*		include space
/* ===================================================================== */
// engine
#include <Engine/Application/UI/EngineUI/IEngineUI.h>
#include <Engine/Editor/BaseEditor.h>

// c++
#include <vector>
#include <string>

// Forward
class EditorContext;

class EditorPanel
	:public IEngineUI{
public:
	//===================================================================*/
	//                   public functions
	//===================================================================*/
	EditorPanel();
	~EditorPanel() = default;

	void Render() override;
	const std::string& GetPanelName() const override;

	void AddEditor(const BaseEditor* editor);
	void RemoveEditor(const BaseEditor* editor);

	void SetEditorContext(EditorContext* context);

private:
	//===================================================================*/
	//                   private variables
	//===================================================================*/
	std::vector<BaseEditor*> editors_;	//< エディタのリスト
	EditorContext* pEditorContext_ = nullptr;
public:
	static int selectedEditorIndex;							//< 選択されたエディタ
};


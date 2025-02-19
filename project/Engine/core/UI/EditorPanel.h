#pragma once

// engine
#include "IEngineUI.h"
#include "Engine/Editor/BaseEditor.h"

// c++
#include <vector>
#include <string>

class EditorPanel
	:public IEngineUI{
public:
	//===================================================================*/
	//                   public functions
	//===================================================================*/
	EditorPanel();
	~EditorPanel() = default;

	void Render(); // 描画処理
	const std::string& GetPanelName() const; // パネル名の取得

	void AddEditor(const BaseEditor* editor);
	void RemoveEditor(const BaseEditor* editor);

private:
	//===================================================================*/
	//                   private variables
	//===================================================================*/
	std::vector<BaseEditor*> editors_;	//< エディタのリスト
public:
	static int selectedEditorIndex;							//< 選択されたエディタ
};


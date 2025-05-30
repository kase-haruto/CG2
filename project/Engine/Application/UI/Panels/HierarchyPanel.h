#pragma once
/* ========================================================================
/*      include space
/* ===================================================================== */
// engine
#include <Engine/Application/UI/EngineUI/IEngineUI.h>

class EditorContext;
class SceneObjectLibrary;

// c++
#include <vector>
#include <string>

class HierarchyPanel
	: public IEngineUI{
public:
	//===================================================================*/
	//                   public functions
	//===================================================================*/
	HierarchyPanel();
	~HierarchyPanel() override = default;

	void Render() override; // 描画処理
	const std::string& GetPanelName() const override; // パネル名の取得

	void SetEditorContext(EditorContext* context);
	void SetSceneObjectLibrary(const SceneObjectLibrary* library);

public:
	static int selectedObjectIndex_;							// 選択中のオブジェクトのインデックス
	EditorContext* pEditorContext_ = nullptr;				// context ポインタ
	const SceneObjectLibrary* pSceneObjectLibrary_ = nullptr;	// シーンオブジェクトライブラリポインタ
};

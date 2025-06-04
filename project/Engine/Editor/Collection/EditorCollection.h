#pragma once
/* ========================================================================
/* include space
/* ===================================================================== */
#include <Engine/Editor/BaseEditor.h>
#include <unordered_map>
#include <memory>

class EngineUICore;
class EditorCollection{
public:
	enum class EditorType{
		PostProcess,
		Effect,
		Ui,
	};
public:
	//===================================================================*/
	//		public functions
	//===================================================================*/
	EditorCollection() = default;
	~EditorCollection() = default;

	void InitializeEditors();
	void UpdateEditors();
	// accessor =========================================================//
	BaseEditor* GetEditor(EditorType editorType);
	void SetEngineUICore(EngineUICore* engineUICore);
private:
	//===================================================================*/
	//		private functions
	//===================================================================*/

private:
	//===================================================================*/
	//		private variables
	//===================================================================*/
	std::unordered_map<EditorType, std::unique_ptr<BaseEditor>> editors_;
	EngineUICore* pEngineUICore_ = nullptr; // EngineUIコアへの参照
};


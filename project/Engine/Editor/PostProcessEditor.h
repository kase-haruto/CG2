#pragma once
/* ========================================================================
/* include space
/* ===================================================================== */
#include <Engine/Editor/BaseEditor.h>

#include <vector>

struct PostEffectSlot;
class PostProcessCollection;

class PostProcessEditor : public
	BaseEditor{
public:
	//===================================================================*/
	// public functions
	//===================================================================*/
	PostProcessEditor(const std::string& name);
	~PostProcessEditor() = default;

	void ShowImGuiInterface() override;
	void SetPostEffectCollection(PostProcessCollection* postProcessCollection);

private:
	//===================================================================*/
	// private variables
	//===================================================================*/
	std::vector<PostEffectSlot> slots_;
	PostProcessCollection* pCollection_ = nullptr;

private:
	const std::string directoryPath_ = "Resources/Json/PostEffect/";
};


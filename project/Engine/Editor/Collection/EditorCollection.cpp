#include "EditorCollection.h"

#include <Engine/Application/UI/Panels/EditorPanel.h>
#include <Engine/Editor/EffectEditor.h>
#include <Engine/Editor/PostProcessEditor.h>
#include <Engine/Editor/UiEditor.h>
#include <Engine/Application/UI/EngineUI/Core/EngineUICore.h>

void EditorCollection::InitializeEditors(){
	EditorPanel* editorPanel = pEngineUICore_->GetEditorPanel();

	//===================================================================*/
	//			postprocess
	//===================================================================*/
	auto postProcessEditor = std::make_unique<PostProcessEditor>("PostProcessEditor");
	editorPanel->AddEditor(postProcessEditor.get());
	editors_.insert({EditorType::PostProcess, std::move(postProcessEditor)});

	//===================================================================*/
	//			EffectEditor
	//===================================================================*/
	auto effectEditor = std::make_unique<EffectEditor>("EffectEditor");
	editorPanel->AddEditor(effectEditor.get());
	editors_.insert({EditorType::Effect, std::move(effectEditor)});

	//===================================================================*/
	//			UiEditor
	//===================================================================*/
	auto uiEditor = std::make_unique<UIEditor>("UiEditor");
	editorPanel->AddEditor(uiEditor.get());
	editors_.insert({EditorType::Ui, std::move(uiEditor)});
}

void EditorCollection::UpdateEditors(){
	
}

BaseEditor* EditorCollection::GetEditor(EditorType editorType){
	auto it = editors_.find(editorType);
	if (it != editors_.end()){
		return it->second.get();
	}
	return nullptr;
}

void EditorCollection::SetEngineUICore(EngineUICore* engineUICore){
	pEngineUICore_ = engineUICore;
}

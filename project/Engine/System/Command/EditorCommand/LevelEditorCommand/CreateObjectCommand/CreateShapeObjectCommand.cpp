#include "CreateShapeObjectCommand.h"

// engine
#include <Engine/Scene/Context/SceneContext.h>

CreateShapeObjectCommand::CreateShapeObjectCommand(SceneContext* context, ObjectFactory factory)
	: context_(context), factory_(std::move(factory)){}

void CreateShapeObjectCommand::Execute() {
	std::unique_ptr<BaseGameObject> tmp = factory_();
	object_ = context_->AddEditorObject(std::move(tmp));
}

void CreateShapeObjectCommand::Undo() {
	context_->RemoveEditorObject(object_);
	object_ = nullptr;
}
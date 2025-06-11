#include "CreateShapeObjectCommand.h"

// engine
#include <Engine/Scene/Context/SceneContext.h>

CreateShapeObjectCommand::CreateShapeObjectCommand(SceneContext* context, ObjectFactory factory)
	: context_(context), factory_(std::move(factory)){}

void CreateShapeObjectCommand::Execute(){
	auto obj = factory_();
	object_ = obj.get();
	context_->AddEditorObject(std::move(obj));
}

void CreateShapeObjectCommand::Undo() {
	context_->RemoveEditorObject(object_);
	object_ = nullptr;
}

const char* CreateShapeObjectCommand::GetName() const{
	return name_.c_str();
}

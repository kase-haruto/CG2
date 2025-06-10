#include "CreateShapeObjectCommand.h"

// engine
#include <Engine/Scene/Context/SceneContext.h>

CreateShapeObjectCommand::CreateShapeObjectCommand(SceneContext* context, ObjectFactory factory)
	: context_(context), factory_(std::move(factory)){}

void CreateShapeObjectCommand::Execute(){
	object_ = factory_(); //< shared_ptr を生成
	context_->AddEditorObject(object_); //< 所有権を共有
}

void CreateShapeObjectCommand::Undo(){
	context_->RemoveEditorObject(object_); //< 同一 shared_ptr により削除可能
}
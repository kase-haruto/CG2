#pragma once

// engine
#include <Engine/System/Command/Interface/ICommand.h>
#include <Engine/Objects/3D/Actor/BaseGameObject.h>

// c++
#include <memory>

class SceneContext;

class CreateShapeObjectCommand 
	: public ICommand{
public:
	using ObjectFactory = std::function<std::shared_ptr<BaseGameObject>()>; //< shared_ptr で生成

	CreateShapeObjectCommand(SceneContext* context, ObjectFactory factory);

	void Execute() override;
	void Undo() override;

private:
	SceneContext* context_ = nullptr;
	ObjectFactory factory_;
	std::shared_ptr<BaseGameObject> object_; //< shared_ptr で保持
};
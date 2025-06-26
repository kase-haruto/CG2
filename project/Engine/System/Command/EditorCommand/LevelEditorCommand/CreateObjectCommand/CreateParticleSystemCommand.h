#pragma once

// engine
#include <Engine/System/Command/Interface/ICommand.h>

// c++
#include <memory>
#include <functional>
#include <string>

class SceneContext;
class ParticleSystemObject;

class CreateParticleSystemObjectCommand
	:public ICommand{
public:
	using ObjectFactory = std::function<std::unique_ptr<ParticleSystemObject>()>;
	CreateParticleSystemObjectCommand(SceneContext* context, ObjectFactory factory);
	
	void Execute() override;
	void Undo() override;
	const char* GetName() const override;

private:
	SceneContext* context_ = nullptr;
	ObjectFactory factory_;
	ParticleSystemObject* particleSystem_ = nullptr;
	std::string name_; // ログ用の名前
};


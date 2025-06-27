#pragma once

// engine
#include <Engine/System/Command/Interface/ICommand.h>

// c++
#include <functional>
#include <string>
#include <memory>
class SceneContext;
class ParticleSystemObject;

class CreateParticleSystemObjectCommand : public ICommand{
public:
	using ObjectFactory = std::function<std::unique_ptr<ParticleSystemObject>()>;

	CreateParticleSystemObjectCommand(SceneContext* context, ObjectFactory factory, std::string name);

	void Execute() override;
	void Undo() override;
	const char* GetName() const override;

private:
	SceneContext* context_ = nullptr;
	ObjectFactory factory_;
	ParticleSystemObject* particleSystem_ = nullptr; // 参照用
	std::string name_;
};

#pragma once

#include "BaseParticle.h"

#include <string>

class Particle
	: public BaseParticle{
public:
	Particle();
	~Particle() override;

	void Initialize(const std::string& modelName, const std::string& texturePath, const uint32_t count) override;

	/* ui ===============================================*/
	void ImGui() override;
	void Load(const std::string& filename);
	void Save(const std::string& filename);

	// getter/setter
	const std::string& GetName() const{ return name_; }
	void SetName(const std::string& name){ name_ = name; }
	bool GetUseRandomColor() const override;
	Vector4 GetSelectedColor() const override;

private:
	std::string fileDirectoryPath = "./Resources/json/particle/";
};

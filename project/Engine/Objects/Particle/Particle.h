#pragma once

#include "BaseParticle.h"
#include <string>

class Particle
	: public BaseParticle{
public:
	Particle();
	~Particle() override;

	Particle(const Particle& other);

	void Initialize(const std::string& modelName, const std::string& texturePath, const uint32_t count) override;
	void LoadInitialize();
	/* ui ===============================================*/
	void ImGui() override;

	/* json =============================================*/
	nlohmann::json SaveToJson() const override;
	void LoadFromJson(const nlohmann::json& j)override;


	// getter/setter
	const std::string& GetName() const{ return name_; }
	void SetName(const std::string& name){ name_ = name; }
	const std::string& GetModelName() const{ return modelName_; }
	const std::string& GetTextureName() const{ return textureName_; }
	bool GetUseRandomColor() const override;
	Vector4 GetSelectedColor() const override;

private:
	std::string fileDirectoryPath = "./Resources/json/particle/";
};

#pragma once

#include "BaseParticle.h"
#include "particleBehavior/PtlBehavior_Diffusion.h"

class DemoParticle 
	: public BaseParticle,public PtlBehavior_Diffusion{
public:
	//===================================================================*/
	//                   public methods
	//===================================================================*/
	DemoParticle() = default;
	~DemoParticle()override = default;

	void Initialize(const std::string& modelName, const uint32_t count)override;

	void Update() override;

	void Draw();

	void Emit(uint32_t count);

};


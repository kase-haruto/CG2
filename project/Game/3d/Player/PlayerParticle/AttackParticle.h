#pragma once

#include "Engine/objects/particle/ParticleSystem.h"
#include "Engine/objects/particle/particleBehavior/PtlBehavior_Diffusion.h"
class AttackParticle 
	: public ParticleSystem{
public:
	//===================================================================*/
	//                   public methods
	//===================================================================*/
	AttackParticle();
	~AttackParticle() override = default;

	void Initialize(const std::string& modelName, const std::string& texturePath) override;
	void Update() override;

private:
	//===================================================================*/
	//                   private methods
	//===================================================================*/
	uint32_t particleNum_;
	float maxScale_ = 6.0f;
};


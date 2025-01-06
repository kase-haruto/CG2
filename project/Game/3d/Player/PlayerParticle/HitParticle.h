#pragma once
#include "Engine/objects/particle/ParticleSystem.h"
#include "Engine/objects/particle/particleBehavior/PtlBehavior_Diffusion.h"

class HitParticle :
    public ParticleSystem{
public:
	//===================================================================*/
	//                   public methods
	//===================================================================*/
	HitParticle();
	~HitParticle() override = default;

	void Initialize(const std::string& modelName, const std::string& texturePath) override;
	void Update() override;

	Vector3 GenerateVelocity(float speed)override;

private:
	//===================================================================*/
	//                   private methods
	//===================================================================*/
	uint32_t particleNum_;
};


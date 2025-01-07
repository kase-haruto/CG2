#pragma once
#include "Engine/objects/particle/ParticleSystem.h"

class TrailParticle :
    public ParticleSystem{
public:
	//===================================================================*/
	//                   public func
	//===================================================================*/
	TrailParticle();
	~TrailParticle()override = default;

	/* main ===================================*/
	void Initialize(const std::string& modelName, const std::string& texturePath) override;
	void Update() override;

private:
	//===================================================================*/
	//                   private methods
	//===================================================================*/
	uint32_t particleNum_;
};


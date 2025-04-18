#pragma once

#include "Engine/objects/particle/Particle.h"
#include "Engine/objects/particle/particleBehavior/PtlBehavior_Tornado.h"

class EnemyDeadParticle
	:public Particle{
public:
	//===================================================================*/
	//                   public methods
	//===================================================================*/
	EnemyDeadParticle();
	~EnemyDeadParticle() override = default;
	void Initialize(const std::string& modelName, const std::string& texturePath) override;
	void Update() override;

private:
	//===================================================================*/
	//                   private methods
	//===================================================================*/
	uint32_t particleNum_;
};


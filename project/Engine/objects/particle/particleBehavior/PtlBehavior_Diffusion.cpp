#include "PtlBehavior_Diffusion.h"

#include "../BaseParticle.h"

//* lib
#include "lib/myFunc/Random.h"


void PtlBehavior_Diffusion::Initialize(BaseParticle& particle){

	for (auto it = particle.particles_.begin(); it != particle.particles_.end(); ){

		if (particle.instanceNum_ < particle.kMaxInstanceNum_){

			it->SetColorRandom();
			it->SetVelocityRandom(-1.0f,1.0f);

		}
	}

}

void PtlBehavior_Diffusion::ApplyBehavior([[maybe_unused]]BaseParticle& particle){
	
}

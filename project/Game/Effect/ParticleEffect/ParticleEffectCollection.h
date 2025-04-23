#pragma once
/* ========================================================================
/*	include space
/* ===================================================================== */

// engine
#include <Game/Effect/ParticleEffect/ParticleEffect.h>

class ParticleEffectCollection{
public:
	//===================================================================*/
	//			public methods
	//===================================================================*/
	ParticleEffectCollection() = default;
	~ParticleEffectCollection() = default;

	void Update();
	void Draw();
	void ImGui();

	//--------- add/remove ------------------------------------------------
	void AddEffect(std::unique_ptr<ParticleEffect> effect);
	void RemoveEffect(size_t index);
	
	//--------- accessor --------------------------------------------------
	std::vector<std::unique_ptr<ParticleEffect>>& GetEffects(){ return effects_; }

private:
	//===================================================================*/
	//			public variables
	//===================================================================*/
	std::vector<std::unique_ptr<ParticleEffect>> effects_;
};


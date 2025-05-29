#pragma once
/* ========================================================================
/*	include space
/* ===================================================================== */

// engine
#include <Game/Effect/ParticleEffect/ParticleEffect.h>

class ParticleEffectCollection{
public:
	//===================================================================*/
	//                      Public Methods
	//===================================================================*/
	ParticleEffectCollection() = default;
	~ParticleEffectCollection() = default;

	void StartupLoad();

	//--------- Add / Remove -----------------------------
	void AddEffect(std::unique_ptr<ParticleEffect> effect);
	void RemoveEffect(size_t index);
	void Clear();

	//--------- Accessor ----------------------------------
	ParticleEffect* GetEffectFromName(const std::string& name);
	void LoadByName(const std::string& name);
	const std::vector<std::unique_ptr<ParticleEffect>>& GetEffects() const { return effects_; }

private:
	std::vector<std::unique_ptr<ParticleEffect>> effects_;
};


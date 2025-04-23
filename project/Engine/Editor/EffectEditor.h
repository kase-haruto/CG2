#pragma once
/* ========================================================================
/* include space
/* ===================================================================== */
#include <Engine/objects/particle/Particle.h>
#include <Game/Effect/ParticleEffect/ParticleEffect.h>
#include <Engine/Editor/BaseEditor.h>

class ParticleEffectCollection;

class EffectEditor :
	public BaseEditor{
public:
	//===================================================================*/
	// public functions
	//===================================================================*/
	EffectEditor(ParticleEffectCollection* collection);
	~EffectEditor() = default;

	void ShowImGuiInterface();
	void SaveToJson(const std::string& filePath);
	void LoadFromJson(const std::string& filePath);

private:
	//===================================================================*/
	// private functions
	//===================================================================*/
	void ShowParticleMakingGui();
	void ShowEffectList();
	void ShowEffectProperty();
	void AddEffect(std::unique_ptr<ParticleEffect> effect);
	void RemoveEffect(int index);

private:
	//===================================================================*/
	// private variables
	//===================================================================*/
	int selectedEffectIndex_ = -1;
	ParticleEffectCollection* effectCollection_ = nullptr;

private:
	const std::string directoryPath_ = "Resources/Json/Effect";
};

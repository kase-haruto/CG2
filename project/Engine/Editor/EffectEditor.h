#pragma once
/* ========================================================================
/* include space
/* ===================================================================== */
#include <Engine/objects/particle/Particle.h>
#include <Game/Effect/ParticleEffect/ParticleEffect.h>
#include <Engine/Editor/BaseEditor.h>

class EffectEditor :
	public BaseEditor {
public:
	//===================================================================*/
	// public functions
	//===================================================================*/
	EffectEditor();
	~EffectEditor()override = default;

	void ShowImGuiInterface()override;
	void SaveToJson(const std::string& filePath);
	void LoadFromJson(const std::string& filePath);
	void LoadFromJsonAll(const std::string& directoryPath);
private:
	//===================================================================*/
	// private functions
	//===================================================================*/
	void ShowParticleMakingGui();
	void ShowEffectListAndProperty();
	void AddEffect(std::unique_ptr<ParticleEffect> effect);
	void RemoveEffect(int index);

private:
	//===================================================================*/
	// private variables
	//===================================================================*/
	int selectedEffectIndex_ = -1;
	ParticleEffect* currentEffect_ = nullptr;
private:
	const std::string directoryPath_ = "Resources/Json/Effect";
};
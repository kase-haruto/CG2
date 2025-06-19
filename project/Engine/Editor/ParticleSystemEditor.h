#pragma once
/* ========================================================================
/* include space
/* ===================================================================== */
#include <Engine/Editor/BaseEditor.h>
#include <Engine/Application/Effects/Particle/Emitter/FxEmitter.h>

/* ========================================================================
/* particle system editor
/* ===================================================================== */
class ParticleSystemEditor :
	public BaseEditor{
public:
	//===================================================================*/
	// public functions
	//===================================================================*/
	void ShowImGuiInterface()override;
	void UpdatePreviewParticles();
	void SaveToFile(const std::string& path);
	void LoadFromFile(const std::string& path);
	FxEmitter* GetPreviewEmitter();

private:
	//===================================================================*/
	// private functions
	//===================================================================*/

private:
	//===================================================================*/
	// private variables
	//===================================================================*/
	std::unique_ptr<FxEmitter> previewEmitter_;
};
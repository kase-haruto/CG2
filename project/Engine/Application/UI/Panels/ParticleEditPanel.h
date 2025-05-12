#pragma once
/* ========================================================================
/*	include space
/* ===================================================================== */
// engine
#include <Engine/Application/UI/EngineUI/IEngineUI.h>
#include <Engine/objects/particle/ParticleManager.h>

// c++
#include <vector>
#include <string>

class ParticleEditPanel
:public IEngineUI{
public:
	//===================================================================*/
	//                   public funclion
	//===================================================================*/
	ParticleEditPanel();
	~ParticleEditPanel() override = default;
	void Render() override;
	const std::string& GetPanelName() const override;

private:
	ParticleManager* particleManager_ = nullptr;

public:
	//===================================================================*/
	//                   getter/setter
	//===================================================================*/
	void SetParticleManager(ParticleManager* particleManager){ particleManager_ = particleManager; }
};


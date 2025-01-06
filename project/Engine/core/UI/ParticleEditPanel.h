#pragma once

#include "IEngineUI.h"
#include "Engine/objects/particle/ParticleManager.h"

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


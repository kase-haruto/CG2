#include "FxSystem.h"

#include <Engine/Application/Effects/Particle/FxUnit.h>


void FxSystem::AddEmitter(std::shared_ptr<FxEmitter> emitter) {
	emitters_.push_back(emitter);
}

void FxSystem::Update(float dt) {
	for (auto& e : emitters_) e->Update(dt);
}

std::vector<FxUnit> FxSystem::CollectAliveUnits() const {
	std::vector<FxUnit> result;
	for (const auto& e : emitters_) {
		for (const auto& fx : e->GetUnits()) {
			if (fx.alive) result.push_back(fx);
		}
	}
	return result;
}
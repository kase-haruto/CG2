#include "FxSystem.h"

#include <Engine/Application/Effects/Particle/FxUnit.h>


void FxSystem::AddEmitter(FxEmitter* emitter) {
	emitters_.push_back(emitter);
}

void FxSystem::Update(float dt) {
	for (auto& e : emitters_) e->Update(dt);
}


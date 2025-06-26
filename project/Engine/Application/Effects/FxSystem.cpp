#include "FxSystem.h"

#include <Engine/Application/Effects/Particle/FxUnit.h>


void FxSystem::AddEmitter(FxEmitter* emitter) {
	emitters_.push_back(emitter);
}

void FxSystem::RemoveEmitter(FxEmitter* emitter){
	auto it = std::remove(emitters_.begin(), emitters_.end(), emitter);
	if (it != emitters_.end()){
		emitters_.erase(it, emitters_.end());
	}
}

void FxSystem::Update() {
	for (auto& e : emitters_) e->Update();
}


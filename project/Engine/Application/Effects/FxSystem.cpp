#include "FxSystem.h"

#include <Engine/Application/Effects/Particle/FxUnit.h>


FxSystem::FxSystem(){
	particleRenderer_ = std::make_unique<ParticleRenderer>();
}

FxSystem::~FxSystem(){
	emitters_.clear();
}

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

void FxSystem::Render(PipelineService* pso, ID3D12GraphicsCommandList* cmd){
	particleRenderer_->Render(emitters_, pso,cmd);
}

void FxSystem::Clear(){
	emitters_.clear();
}


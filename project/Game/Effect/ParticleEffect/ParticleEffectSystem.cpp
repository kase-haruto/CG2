#include "ParticleEffectSystem.h"

ParticleEffectSystem* ParticleEffectSystem::GetInstance() {
	static ParticleEffectSystem instance;
	return &instance;
}

void ParticleEffectSystem::Update() {
	for (auto& effect : activeEffects_) {
		effect->Update();
	}

	if (editorPreviewEffect_) {
		editorPreviewEffect_->Update();
	}

	//activeEffects_.erase(
	//	std::remove_if(activeEffects_.begin(), activeEffects_.end(),
	//				   [](const std::unique_ptr<ParticleEffect>& e) {
	//	return e->IsFinished();
	//}),
	//	activeEffects_.end());
}

void ParticleEffectSystem::Draw(ID3D12GraphicsCommandList* cmdList) {
	for (auto& effect : activeEffects_) {
		effect->Draw(cmdList);
	}

	if (editorPreviewEffect_) {
		editorPreviewEffect_->Draw(cmdList);
	}
}

void ParticleEffectSystem::Finalize() { 
	activeEffects_.clear();
	collection_.Clear();
}

void ParticleEffectSystem::PlayByName(const std::string& name, const Vector3& position, EmitType emitType) {
	auto prototype = collection_.GetEffectFromName(name);
	if (prototype) {
		auto newEffect = std::make_unique<ParticleEffect>(*prototype);
		newEffect->Initialize();
		newEffect->SetPosition(position);
		newEffect->Play(position, emitType);
		activeEffects_.push_back(std::move(newEffect));
	}
}

ParticleEffect* ParticleEffectSystem::CreateEffectByName(const std::string& name, const Vector3& position, EmitType emitType) {
	auto prototype = collection_.GetEffectFromName(name);
	if (prototype) {
		auto newEffect = std::make_unique<ParticleEffect>(*prototype);
		newEffect->Initialize();
		newEffect->SetPosition(position);
		newEffect->Play(position, emitType);
		activeEffects_.push_back(std::move(newEffect));
		return activeEffects_.back().get();
	}
	return nullptr;
}

void ParticleEffectSystem::PlayForEditorPreview(std::unique_ptr<ParticleEffect> effect) {
	effect->Play(Vector3::Zero,EmitType::Both);
	activeEffects_.push_back(std::move(effect));
}


void ParticleEffectSystem::SetEditorPreviewPointer(ParticleEffect* effect) {
	editorPreviewEffect_ = effect;
}
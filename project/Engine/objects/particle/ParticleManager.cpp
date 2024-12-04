#include "ParticleManager.h"

#include "Engine/graphics/GraphicsGroup.h"
#include "Engine/graphics/SrvLocator.h"

#include "lib/myFunc/MyFunc.h"

// ParticleGroup の初期化処理
void ParticleGroup::Initialize(const std::string& texturePath){
	textureFilePath = texturePath;
}

void ParticleGroup::Update(){

	particles->Update();

}

void ParticleGroup::Draw(){

	particles->Draw();

}

ParticleManager* ParticleManager::GetInstance(){
	static ParticleManager instance;
	return &instance;
}

void ParticleManager::CreateParticleGroup(const std::string& name, const std::string& textureFilePath){
	if (particleGroups_.find(name) != particleGroups_.end()){
		throw std::runtime_error("ParticleGroup with this name already exists.");
	}

	ParticleGroup group;
	group.Initialize(textureFilePath);
	particleGroups_.emplace(name, std::move(group));
}

void ParticleManager::Update(){
	for (auto& [name, group] : particleGroups_){
		group.Update();
	}
}

void ParticleManager::Draw(){

	for (auto& [name, group] : particleGroups_){
		group.Draw();
	}

}

void ParticleManager::Finalize(){}


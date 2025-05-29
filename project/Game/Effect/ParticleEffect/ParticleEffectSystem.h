#pragma once

#include <Game/Effect/ParticleEffect/ParticleEffect.h>
#include <Game/Effect/ParticleEffect/ParticleEffectCollection.h>

#include <vector>
#include <memory>


class ParticleEffectSystem {
public:
	 //===================================================================*/
	 //					singleton methods
	 //===================================================================*/
	static ParticleEffectSystem* GetInstance();
public:
	//===================================================================*/
	//					public functions
	//===================================================================*/
	void Update();
	void Draw(ID3D12GraphicsCommandList* cmdList);
	void Finalize();
	void PlayByName(const std::string& name, const Vector3& position, EmitType emitType = EmitType::Once);
	ParticleEffect* CreateEffectByName(const std::string& name, const Vector3& position, EmitType emitType);
	void PlayForEditorPreview(std::unique_ptr<ParticleEffect> effect);
	//--------- Accessor ----------------------------------
	ParticleEffectCollection& GetCollection() { return collection_; }

private:
	//===================================================================*/
	//					private functions
	//===================================================================*/
	ParticleEffectSystem() = default;

private:
	//===================================================================*/
	//					private variables
	//===================================================================*/
	std::vector<std::unique_ptr<ParticleEffect>> activeEffects_;
	ParticleEffectCollection collection_;
};

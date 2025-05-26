#pragma once
/* ========================================================================
/* include space
/* ===================================================================== */

// engine
#include <Engine/objects/particle/Particle.h>

// c++
#include <string>
#include <memory>
#include <vector>

/* ========================================================================
/* particleEffect
/* ===================================================================== */
class ParticleEffect{
public:
	//===================================================================*/
	//		public func
	//===================================================================*/
	ParticleEffect() = default;
	~ParticleEffect() = default;

	void Initialize();
	void Update();
	void Draw();
	void ImGui();
	bool IsFinished()const;
	void Play(const Vector3& pos, EmitType emitType);

	//--------- add/remove -----------------------------------------------
	void AddParticle(std::unique_ptr<Particle> system);
	void RemoveParticle(size_t index);

	//--------- json -----------------------------------------------------
	void Save(const std::string& filename);
	void Load(const std::string& filename);

	void SetName(const std::string& name){ name_ = name; }
	const std::string& GetName()const { return name_; }
	void SetPendingDelete(bool pendingDelete){ pendingDelete_ = pendingDelete; }
	void SetPosition(const Vector3& position){
		for (auto& particle : particles_){
			particle->SetEmitPos(position);
		}
	}
	bool IsPendingDelete() const { return pendingDelete_; }
	bool IsPlaying() const { return isPlaying_; }
private:
	//===================================================================*/
	//		private func
	//===================================================================*/
	std::string name_ = "UnnamedEffect";
	std::vector<std::unique_ptr<Particle>> particles_;
	bool pendingDelete_ = false;
	bool isPlaying_ = false;
};


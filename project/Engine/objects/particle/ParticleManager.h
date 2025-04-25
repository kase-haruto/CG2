#pragma once

#include <vector>
#include <memory>
#include "Particle.h"

class ParticleManager{
public:
	//===================================================================*/
	//                   private Methods
	//===================================================================*/
	ParticleManager();
	~ParticleManager() = default;

public:
	//===================================================================*/
	//                   public Methods
	//===================================================================*/

	// 粒子システムを追加
	void AddParticle(std::unique_ptr<Particle>&& particle);
	// インデックス指定で削除
	bool RemoveParticleAt(int index);

	// ポインタ指定で削除（Particle* と一致するものを探す）
	bool RemoveParticle(Particle* target);

	void Update();
	// 描画処理
	void Draw();

	void Finalize();

	void ShowDebugUI();

	//--------- accessor -----------------------------------------------------
	const std::vector<std::unique_ptr<Particle>>& GetParticles() const{return particles_;} // 粒子システムの取得
	Particle* GetParticle(int index){ return particles_[index].get(); } // 指定したインデックスの粒子システムを取得

private:
	std::vector<std::unique_ptr<Particle>> particles_; // 管理する粒子システム
};
#pragma once
/* ========================================================================
/*	include space
/* ===================================================================== */

// engine
#include <Game/Effect/ParticleEffect/ParticleEffect.h>

class ParticleEffectCollection{
public:
	//===================================================================*/
	//                      Singleton Access
	//===================================================================*/
	static ParticleEffectCollection* GetInstance();

	//===================================================================*/
	//                      Public Methods
	//===================================================================*/
	~ParticleEffectCollection() = default;

	void StartupLoad();
	void Update();
	void Draw();
	void PlayByName(const std::string& name, const Vector3& position, EmitType emitType = EmitType::Once);

	//--------- Add / Remove -----------------------------
	void AddEffect(std::unique_ptr<ParticleEffect> effect); // エフェクトを追加（初期化含む）
	void RemoveEffect(size_t index); // 指定インデックスのエフェクトを削除
	void Clear(); // 全削除

	//--------- Accessor ----------------------------------
	ParticleEffect* GetEffectFromName(const std::string& name); // 名前からエフェクトを取得
	const std::vector<std::unique_ptr<ParticleEffect>>& GetEffects() const{ return effects_; }

private:
	//===================================================================*/
	//                      Private Members
	//===================================================================*/
	ParticleEffectCollection() = default;

	std::vector<std::unique_ptr<ParticleEffect>> effects_; // 登録された全エフェクト
};


#include "ParticleEffectCollection.h"
/* ========================================================================
/*	include space
/* ===================================================================== */
#include <Engine/Graphics/Context/GraphicsGroup.h>
// externals
#include <externals/imgui/imgui.h>
#include <filesystem>
namespace fs = std::filesystem;

//===================================================================*/
//			初期読み込み
//===================================================================*/
void ParticleEffectCollection::StartupLoad() {
	const std::string directoryPath = "Resources/Json/Effect";
	Clear();

	for (const auto& entry : fs::directory_iterator(directoryPath)) {
		if (entry.is_regular_file() && entry.path().extension() == ".json") {
			auto effect = std::make_unique<ParticleEffect>();
			effect->Load(entry.path().string());
			AddEffect(std::move(effect));
		}
	}
}

//===================================================================*/
//			追加
//===================================================================*/
void ParticleEffectCollection::AddEffect(std::unique_ptr<ParticleEffect> effect) {
	if (effect) {
		effect->Initialize();
		effects_.emplace_back(std::move(effect));
	}
}

//===================================================================*/
//			名前からしゅとく
//===================================================================*/
ParticleEffect* ParticleEffectCollection::GetEffectFromName(const std::string& name) {
	for (auto& effect : effects_) {
		if (effect->GetName() == name) {
			return effect.get();
		}
	}
	return nullptr;
}

void ParticleEffectCollection::LoadByName(const std::string& name) {
	if (GetEffectFromName(name)) return;

	const std::string directoryPath = "Resources/Json/Effect/";
	auto effect = std::make_unique<ParticleEffect>();
	effect->Load(directoryPath + name + ".json");
	effect->Initialize();
	AddEffect(std::move(effect));
}

//===================================================================*/
//			削除
//===================================================================*/
void ParticleEffectCollection::RemoveEffect(size_t index) {
	if (index < effects_.size()) {
		effects_.erase(effects_.begin() + index);
	}
}

//===================================================================*/
//			クリア
//===================================================================*/
void ParticleEffectCollection::Clear() {
	effects_.clear();
}
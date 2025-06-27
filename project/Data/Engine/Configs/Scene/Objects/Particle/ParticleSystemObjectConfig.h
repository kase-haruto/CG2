#pragma once

#include <Data/Engine/Configs/Scene/Objects/SceneObject/SceneObjectConfig.h>
#include <Data/Engine/Configs/Scene/Objects/Particle/EmitterConfig.h>

struct ParticleSystemObjectConfig 
	: public SceneObjectConfig, public EmitterConfig{

};

// JSON変換
inline void to_json(nlohmann::json& j, const ParticleSystemObjectConfig& c){
	j = nlohmann::json {
		// SceneObjectConfig部分
		{"guid",        c.guid},
		{"parentGuid",  c.parentGuid},
		{"objectType",  c.objectType},

		{"name",        c.name},
		{"transform",   c.transform},

		// EmitterConfig部分
		{"modelPath",   c.modelPath},
		{"texturePath", c.texturePath},
		{"emitRate",    c.emitRate},
		{"velocity",    c.velocity},
		{"lifetime",    c.lifetime},
		{"isComplement", c.isComplement},
		{"isStatic",    c.isStatic}
	};
}

inline void from_json(const nlohmann::json& j, ParticleSystemObjectConfig& c){
	// SceneObjectConfig
	j.at("guid").get_to(c.guid);
	j.at("parentGuid").get_to(c.parentGuid);
	j.at("objectType").get_to(c.objectType);
	j.at("name").get_to(c.name);
	j.at("transform").get_to(c.transform);

	// EmitterConfig
	j.at("modelPath").get_to(c.modelPath);
	j.at("texturePath").get_to(c.texturePath);
	j.at("emitRate").get_to(c.emitRate);
	j.at("velocity").get_to(c.velocity);
	j.at("lifetime").get_to(c.lifetime);
	j.at("isComplement").get_to(c.isComplement);
	j.at("isStatic").get_to(c.isStatic);
}

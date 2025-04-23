#include "Particle.h"

#include "ParticleManager.h"
#include <lib/myFunc/Random.h>

// lib
#include <externals/imgui/imgui.h>
#include <externals/nlohmann/json.hpp>
#include <fstream>
#include <algorithm>
using json = nlohmann::json;


Particle::Particle(){}

Particle::~Particle(){}

void Particle::Initialize(const std::string& modelName, const std::string& texturePath, const uint32_t count){
	//Load(fileDirectoryPath + GetName() + ".json");

	BaseParticle::Initialize(modelName, texturePath, count);

}

//===================================================================*/
//                    json/ui
//===================================================================*/
void Particle::ImGui(){
	if (ImGui::Button("Save to JSON")){
		Save(fileDirectoryPath + GetName() + ".json");
	}

	if (ImGui::TreeNode("Visual Setting")){
		BaseParticle::VisualSettingGui();
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Emitter Parameter")){
		BaseParticle::EmitterGui();
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Particle Parameter")){
		BaseParticle::ParameterGui();
		ImGui::TreePop();
	}
}

void Particle::Save(const std::string& filename){
	json j;
	j["name"] = name_;
	j["colorMode"] = static_cast< int >(colorMode_);
	j["selectedColor"] = {selectedColor_.x, selectedColor_.y, selectedColor_.z, selectedColor_.w};
	j["colorVariation"] = colorVariation_;

	json emitterArray = json::array();
	for (const auto& emitter : emitters_){
		json e;
		e["count"] = emitter.count;
		e["frequency"] = emitter.frequency;
		e["frequencyTime"] = emitter.frequencyTime;
		e["shape"] = static_cast< int >(emitter.shape);
		e["transform"] = {
			{"translate", {emitter.transform.translate.x, emitter.transform.translate.y, emitter.transform.translate.z}},
			{"rotate", {emitter.transform.rotate.x, emitter.transform.rotate.y, emitter.transform.rotate.z}},
			{"scale", {emitter.transform.scale.x, emitter.transform.scale.y, emitter.transform.scale.z}}
		};
		emitterArray.push_back(e);
	}
	j["Emitters"] = emitterArray;

	std::ofstream ofs(filename);
	if (ofs){
		ofs << j.dump(4);
	}
}

void Particle::Load(const std::string& filename){
	std::ifstream ifs(filename);
	if (!ifs) return;

	json j;
	ifs >> j;

	if (j.contains("name")) name_ = j["name"].get<std::string>();
	if (j.contains("colorMode")) colorMode_ = static_cast< ColorMode >(j["colorMode"].get<int>());
	if (j.contains("selectedColor")){
		auto col = j["selectedColor"].get<std::vector<float>>();
		if (col.size() == 4) selectedColor_ = {col[0], col[1], col[2], col[3]};
	}
	if (j.contains("colorVariation")) colorVariation_ = j["colorVariation"].get<float>();

	emitters_.clear();
	if (j.contains("Emitters")){
		for (const auto& e : j["Emitters"]){
			ParticleData::Emitter emitter;
			emitter.count = e.value("count", 10);
			emitter.frequency = e.value("frequency", 0.5f);
			emitter.frequencyTime = e.value("frequencyTime", 0.0f);
			emitter.shape = static_cast< EmitterShape >(e.value("shape", 1));
			if (e.contains("transform")){
				auto t = e["transform"];
				auto tr = t.value("translate", std::vector<float>{0, 0, 0});
				auto ro = t.value("rotate", std::vector<float>{0, 0, 0});
				auto sc = t.value("scale", std::vector<float>{1, 1, 1});
				if (tr.size() == 3) emitter.transform.translate = {tr[0], tr[1], tr[2]};
				if (ro.size() == 3) emitter.transform.rotate = {ro[0], ro[1], ro[2]};
				if (sc.size() == 3) emitter.transform.scale = {sc[0], sc[1], sc[2]};
			}
			emitters_.push_back(emitter);
		}
	}
}


bool Particle::GetUseRandomColor() const{
	return (colorMode_ == ColorMode::Random);
}

Vector4 Particle::GetSelectedColor() const{
	if (colorMode_ == ColorMode::SingleColor){
		return selectedColor_;
	} else if (colorMode_ == ColorMode::SimilarColor){
		Vector4 c = selectedColor_;
		c.x = std::clamp(c.x + Random::Generate(-colorVariation_, colorVariation_), 0.0f, 1.0f);
		c.y = std::clamp(c.y + Random::Generate(-colorVariation_, colorVariation_), 0.0f, 1.0f);
		c.z = std::clamp(c.z + Random::Generate(-colorVariation_, colorVariation_), 0.0f, 1.0f);
		return c;
	}
	return {1.0f, 1.0f, 1.0f, 1.0f};
}
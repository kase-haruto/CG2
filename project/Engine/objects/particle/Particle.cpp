#include "Particle.h"

#include "Engine/objects/TextureManager.h"
#include <lib/myFunc/Random.h>
#include <lib/myFunc/MyFunc.h>

// lib
#include <externals/imgui/imgui.h>
#include <externals/nlohmann/json.hpp>
#include <fstream>
#include <algorithm>
#include <numbers>

using json = nlohmann::json;


Particle::Particle(){}

Particle::~Particle(){}

void Particle::Initialize(const std::string& modelName, const std::string& texturePath, const uint32_t count){
	//Load(fileDirectoryPath + GetName() + ".json");

	BaseParticle::Initialize(modelName, texturePath, count);

}

void Particle::LoadInitialize(){
	textureHandle = TextureManager::GetInstance()->LoadTexture(textureName_);
	backToFrontMatrix_ = MakeRotateYMatrix(std::numbers::pi_v<float>);
	instanceDataList_.reserve(kMaxInstanceNum_);
	BaseParticle::CreateBuffer();
}

//===================================================================*/
//                    json/ui
//===================================================================*/
void Particle::ImGui(){

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

nlohmann::json Particle::SaveToJson() const{
	nlohmann::json json;
	json["name"] = name_;
	json["model"] = modelName_;
	json["texture"] = textureName_;
	json["isStatic"] = isStatic_;
	json["isAutoEmit"] = autoEmit_;
	json["isBillboard"] = isBillboard_;
	json["useRotation"] = useRotation_;
	json["useRandomScale"] = useRandomScale_;
	json["fixedMaxScale"] = {fixedMaxScale_.x, fixedMaxScale_.y, fixedMaxScale_.z};
	json["randomScaleMin"] = {randomScaleMin_.x, randomScaleMin_.y, randomScaleMin_.z};
	json["randomScaleMax"] = {randomScaleMax_.x, randomScaleMax_.y, randomScaleMax_.z};
	json["blendMode"] = static_cast< int >(blendMode_);
	json["colorMode"] = static_cast< int >(colorMode_);
	json["selectedColor"] = {selectedColor_.x, selectedColor_.y, selectedColor_.z, selectedColor_.w};
	json["colorVariation"] = colorVariation_;
	json["lifeTime"] = lifeTime_;

	// Emitters
	nlohmann::json emittersJson = nlohmann::json::array();
	for (const auto& e : emitters_){
		nlohmann::json emitterJson;
		emitterJson["count"] = e.count;
		emitterJson["frequency"] = e.frequency;
		emitterJson["shape"] = static_cast< int >(e.shape);
		emitterJson["transform"] = {
			{"translate", {e.transform.translate.x, e.transform.translate.y, e.transform.translate.z}},
			{"scale", {e.transform.scale.x, e.transform.scale.y, e.transform.scale.z}},
			{"rotate", {e.transform.rotate.x, e.transform.rotate.y, e.transform.rotate.z}},
		};
		emitterJson["rotationParm"] = {
			{"useRotation", e.parmData.useRotation},
			{"rotateContinuously", e.parmData.rotateContinuously},
			{"randomizeInitialRotation", e.parmData.randomizeInitialRotation},
			{"initialRotation", {e.parmData.initialRotation.x, e.parmData.initialRotation.y, e.parmData.initialRotation.z}},
			{"rotationSpeed", {e.parmData.rotationSpeed.x, e.parmData.rotationSpeed.y, e.parmData.rotationSpeed.z}}
		};
		emittersJson.push_back(emitterJson);
	}
	json["emitters"] = emittersJson;

	return json;
}

void Particle::LoadFromJson(const nlohmann::json& j){
	name_ = j.value("name", "");
	modelName_ = j.value("model", "plane.obj");
	textureName_ = j.value("texture", "particle");
	autoEmit_ = j.value("isAutoEmit", true);
	isBillboard_ = j.value("isBillboard", true);
	useRotation_ = j.value("useRotation", false);
	blendMode_ = static_cast< BlendMode >(j.value("blendMode", static_cast< int >(BlendMode::ADD)));
	colorMode_ = static_cast< ColorMode >(j.value("colorMode", 0));
	lifeTime_ = j.value("lifeTime", 1.0f);
	isStatic_ = j.value("isStatic", false);
	useRandomScale_ = j.value("useRandomScale", false);
	randomScaleMin_ = {j.value("randomScaleMin", std::vector<float>{1.0f, 1.0f, 1.0f})[0],
		j.value("randomScaleMin", std::vector<float>{1.0f, 1.0f, 1.0f})[1],
		j.value("randomScaleMin", std::vector<float>{1.0f, 1.0f, 1.0f})[2]};
	randomScaleMax_ = {j.value("randomScaleMax", std::vector<float>{1.0f, 1.0f, 1.0f})[0],
		j.value("randomScaleMax", std::vector<float>{1.0f, 1.0f, 1.0f})[1],
		j.value("randomScaleMax", std::vector<float>{1.0f, 1.0f, 1.0f})[2]};
	auto col = j.value("selectedColor", std::vector<float>{1.0f, 1.0f, 1.0f, 1.0f});
	if (col.size() == 4) selectedColor_ = {col[0], col[1], col[2], col[3]};
	colorVariation_ = j.value("colorVariation", 0.1f);

	emitters_.clear();
	for (const auto& ej : j["emitters"]){
		ParticleData::Emitter emitter;
		emitter.count = ej["count"];
		emitter.frequency = ej["frequency"];
		emitter.shape = static_cast< EmitterShape >(ej["shape"]);

		auto tr = ej["transform"]["translate"];
		auto sc = ej["transform"]["scale"];
		auto ro = ej["transform"]["rotate"];
		emitter.transform.translate = {tr[0], tr[1], tr[2]};
		emitter.transform.scale = {sc[0], sc[1], sc[2]};
		emitter.transform.rotate = {ro[0], ro[1], ro[2]};

		auto rp = ej["rotationParm"];
		emitter.parmData.useRotation = rp["useRotation"];
		emitter.parmData.rotateContinuously = rp["rotateContinuously"];
		emitter.parmData.randomizeInitialRotation = rp["randomizeInitialRotation"];
		auto ir = rp["initialRotation"];
		emitter.parmData.initialRotation = {ir[0], ir[1], ir[2]};
		auto rs = rp["rotationSpeed"];
		emitter.parmData.rotationSpeed = {rs[0], rs[1], rs[2]};

		emitters_.push_back(emitter);
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
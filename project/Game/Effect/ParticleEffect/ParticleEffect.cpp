#include "ParticleEffect.h"
/* ========================================================================
/*		include space
/* ===================================================================== */
// engine
#include <Engine/Graphics/Context/GraphicsGroup.h>

// c++/lib
#include <externals/imgui/imgui.h>
#include <externals/nlohmann/json.hpp>
#include <fstream>

#include <filesystem>
namespace fs = std::filesystem;

ParticleEffect::ParticleEffect(const ParticleEffect& other){
	name_ = other.name_;
	pendingDelete_ = other.pendingDelete_;
	isPlaying_ = other.isPlaying_;

	// Particle（Emitterなどを含む）もコピー
	for (const auto& p : other.particles_){
		auto cloned = std::make_unique<Particle>(*p);
		particles_.push_back(std::move(cloned));
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
//		初期化
/////////////////////////////////////////////////////////////////////////////////////////
void ParticleEffect::Initialize(){
	for (auto& ps : particles_){
		ps->Initialize(ps->GetModelName(), ps->GetTextureName(), 1);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////
//		更新
///////////////////////////////////////////////////////////////////////////////////////////
void ParticleEffect::Update(){
	for (auto& ps : particles_){
		ps->Update();
	}

	 // 全ての Particle が終了していたら、isPlaying_ を false にする
	if (IsFinished()) {
		isPlaying_ = false;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////
//		描画
///////////////////////////////////////////////////////////////////////////////////////////
void ParticleEffect::Draw(ID3D12GraphicsCommandList* cmdList){
	for (auto& p : particles_){
		auto blendMode = p->GetBlendMode();
		auto rs = GraphicsGroup::GetInstance()->GetRootSignature(PipelineType::StructuredObject, blendMode);
		auto ps = GraphicsGroup::GetInstance()->GetPipelineState(PipelineType::StructuredObject, blendMode);
		cmdList->SetGraphicsRootSignature(rs.Get());
		cmdList->SetPipelineState(ps.Get());

		p->Draw(cmdList);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
//		imgui
/////////////////////////////////////////////////////////////////////////////////////////
void ParticleEffect::ImGui(){
	ImGui::Text("Effect Name: %s", name_.c_str());
	for (size_t i = 0; i < particles_.size(); ++i){
		ImGui::PushID(static_cast< int >(i));
		if (ImGui::CollapsingHeader("Particle System", ImGuiTreeNodeFlags_DefaultOpen)){
			particles_[i]->ImGui();
			if (ImGui::Button("Remove")){
				RemoveParticle(i);
				ImGui::PopID();
				break;
			}
		}
		ImGui::PopID();
	}
	if (ImGui::Button("Add New System")){
		auto newSystem = std::make_unique<Particle>();
		AddParticle(std::move(newSystem));
	}
}

bool ParticleEffect::IsFinished() const{
	for (const auto& p : particles_){
		if (!p->particles_.empty()){
			return false;
		}
	}
	return true;
}

void ParticleEffect::Play(const Vector3& pos, EmitType emitType){
	for (auto& p: particles_){
		p->SetEmitPos(pos);
		p->Play(emitType);
	}
	isPlaying_ = true;
}

/////////////////////////////////////////////////////////////////////////////////////////
//		追加
/////////////////////////////////////////////////////////////////////////////////////////
void ParticleEffect::AddParticle(std::unique_ptr<Particle> particle){
	particle->Initialize("plane.obj", "particle.png", 1);
	particles_.emplace_back(std::move(particle));
}

/////////////////////////////////////////////////////////////////////////////////////////
//		解放
/////////////////////////////////////////////////////////////////////////////////////////
void ParticleEffect::RemoveParticle(size_t index){
	if (index < particles_.size()){
		particles_.erase(particles_.begin() + index);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
//		保存
/////////////////////////////////////////////////////////////////////////////////////////
void ParticleEffect::Save(const std::string& filename){
	fs::path path(filename);
	if (path.has_parent_path()){
		fs::create_directories(path.parent_path());
	}
	nlohmann::json root;
	root["name"] = name_;
	nlohmann::json particleArray = nlohmann::json::array();

	for (const auto& p : particles_){
		particleArray.push_back(p->SaveToJson());
	}

	root["particles"] = particleArray;

	std::ofstream ofs(filename);
	if (ofs) ofs << root.dump(4);
}

void ParticleEffect::Load(const std::string& filename){
	std::ifstream ifs(filename);
	if (!ifs) return;

	nlohmann::json root;
	ifs >> root;
	name_ = root.value("name", "UnnamedEffect");

	particles_.clear();
	for (const auto& j : root["particles"]){
		auto p = std::make_unique<Particle>();
		p->LoadFromJson(j);        // JSONパラメータを読み込む（modelName_, textureName_などを反映）
		p->LoadInitialize();       // 読み込み用初期化（バッファ類のみ構築）
		particles_.push_back(std::move(p));
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
//		読み込み
/////////////////////////////////////////////////////////////////////////////////////////

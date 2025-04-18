#include "ParticleManager.h"

#include "Engine/graphics/GraphicsGroup.h"

// external
#include <externals/imgui/imgui.h>

ParticleManager::ParticleManager(){
	particles_.clear();
}


void ParticleManager::AddParticle(std::unique_ptr<Particle>&& particle){
	if (particle){
		particles_.emplace_back(std::move(particle));
	}
}

bool ParticleManager::RemoveParticleAt(int index){
	if (index >= 0 && index < static_cast< int >(particles_.size())){
		particles_.erase(particles_.begin() + index);
		return true;
	}
	return false;
}



bool ParticleManager::RemoveParticle(Particle* target){
	auto it = std::find_if(particles_.begin(), particles_.end(),
						   [target] (const std::unique_ptr<Particle>& p){
							   return p.get() == target;
						   });
	if (it != particles_.end()){
		particles_.erase(it);
		return true;
	}
	return false;
}

void ParticleManager::Update(){
	for (auto& particle : particles_){
		particle->Update();
	}
}

void ParticleManager::Draw(){
	
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>commandList = GraphicsGroup::GetInstance()->GetCommandList();

	// プリミティブトポロジーを設定（ここでは三角形リストを指定）
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	for (const auto& particle : particles_){
		BlendMode blendMode = particle->GetBlendMode();
		Microsoft::WRL::ComPtr<ID3D12RootSignature>rootSignature = GraphicsGroup::GetInstance()->GetRootSignature(StructuredObject, blendMode);
		Microsoft::WRL::ComPtr<ID3D12PipelineState>pipelineState = GraphicsGroup::GetInstance()->GetPipelineState(StructuredObject, blendMode);
		
		// ルートシグネチャを設定
		commandList->SetGraphicsRootSignature(rootSignature.Get());

		// パイプラインステートを設定
		commandList->SetPipelineState(pipelineState.Get());

		particle->Draw();
	}
}

void ParticleManager::Finalize(){

}

void ParticleManager::ShowDebugUI(){

	// ユニークなIDスコープを作成する
	for (size_t i = 0; i < particles_.size(); ++i){

		// 各オブジェクトのUIを個別に管理
		if (ImGui::CollapsingHeader(particles_[i]->GetName().c_str())){
			particles_[i]->ImGui();
		}

	}
}

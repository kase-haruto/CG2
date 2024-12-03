#include "ParticleManager.h"

#include "Engine/graphics/GraphicsGroup.h"

ParticleManager::ParticleManager(){

	systems_.clear();

}

ParticleManager* ParticleManager::GetInstance(){
    static ParticleManager instance;
    return &instance;
}

void ParticleManager::AddSystem(ParticleSystem* system){
    systems_.push_back(system);
}

void ParticleManager::Draw(){
	Microsoft::WRL::ComPtr<ID3D12RootSignature>rootSignature = GraphicsGroup::GetInstance()->GetRootSignature(StructuredObject);
	Microsoft::WRL::ComPtr<ID3D12PipelineState>pipelineState = GraphicsGroup::GetInstance()->GetPipelineState(StructuredObject);
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>commandList = GraphicsGroup::GetInstance()->GetCommandList();

	// ルートシグネチャを設定
	commandList->SetGraphicsRootSignature(rootSignature.Get());

	// パイプラインステートを設定
	commandList->SetPipelineState(pipelineState.Get());

	// プリミティブトポロジーを設定（ここでは三角形リストを指定）
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    for (const auto& system : systems_){
        system->Draw();
    }
}

void ParticleManager::Finalize(){

	systems_.clear();

}

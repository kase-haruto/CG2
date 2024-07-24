#include "PipelineStateManager.h"

PipelineStateManager::~PipelineStateManager(){}

bool PipelineStateManager::CreatePipelineState(const std::string& name, const std::wstring& vsPath, const std::wstring& psPath,
											   const D3D12_ROOT_SIGNATURE_DESC& rootSignatureDesc,
											   const D3D12_GRAPHICS_PIPELINE_STATE_DESC& psoDesc){
	auto pipelineState = std::make_unique<PipelineState>(device_, shaderManager_);
	pipelineState->Initialize(vsPath, psPath, rootSignatureDesc, psoDesc);
	pipelineStates_[name] = std::move(pipelineState);
	return true;


}

const ComPtr<ID3D12PipelineState>& PipelineStateManager::GetPipelineState(const std::string& name) const{
	auto it = pipelineStates_.find(name);
	assert(it != pipelineStates_.end());
	return it->second->GetPipelineState();
}

const ComPtr<ID3D12RootSignature>& PipelineStateManager::GetRootSignature(const std::string& name) const{
	auto it = pipelineStates_.find(name);
	assert(it != pipelineStates_.end());
	return it->second->GetRootSignature();
}
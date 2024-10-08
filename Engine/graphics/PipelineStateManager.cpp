﻿#include "PipelineStateManager.h"


PipelineStateManager::~PipelineStateManager(){}

bool PipelineStateManager::CreatePipelineState(const PipelineType& type, const std::wstring& vsPath, const std::wstring& psPath,
											   const D3D12_ROOT_SIGNATURE_DESC& rootSignatureDesc,
											   const D3D12_GRAPHICS_PIPELINE_STATE_DESC& psoDesc){
	auto pipelineState = std::make_unique<PipelineState>(device_, shaderManager_);
	if (!pipelineState->Initialize(vsPath, psPath, rootSignatureDesc, psoDesc)){
		// 初期化に失敗した場合
		return false;
	}
	pipelineStates_[type] = std::move(pipelineState);
	return true;
}


void PipelineStateManager::Finalize(){
	shaderManager_.reset();
	device_.Reset();
	for (auto& pipeline:pipelineStates_){
		pipeline.second.reset();
	}
}

const ComPtr<ID3D12PipelineState>& PipelineStateManager::GetPipelineState(const PipelineType& type) const{
	auto it = pipelineStates_.find(type);
	assert(it != pipelineStates_.end());
	return it->second->GetPipelineState();
}

const ComPtr<ID3D12RootSignature>& PipelineStateManager::GetRootSignature(const PipelineType& type) const{
	auto it = pipelineStates_.find(type);
	assert(it != pipelineStates_.end());
	return it->second->GetRootSignature();
}
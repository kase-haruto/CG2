#include "GraphicsGroup.h"

GraphicsGroup* GraphicsGroup::GetInstance(){
	static GraphicsGroup instance;
	return& instance;
}

void GraphicsGroup::Initialize(const DxCore* dxCore, PipelineStateManager* psManager){
	pDxCore_ = dxCore;
	pipelineManager_ = psManager;
}

PipelineStateManager* GraphicsGroup::GetPipelineState()const{ return pipelineManager_; }

const ComPtr<ID3D12PipelineState>& GraphicsGroup::GetPipelineState(const PipelineType& type)const { return pipelineManager_->GetPipelineState(type); }

const ComPtr<ID3D12RootSignature>& GraphicsGroup::GetRootSignature(const PipelineType& type)const{ return pipelineManager_->GetRootSignature(type); }

const ComPtr<ID3D12Device>& GraphicsGroup::GetDevice() const{ return pDxCore_->GetDevice(); }

const ComPtr<ID3D12GraphicsCommandList>& GraphicsGroup::GetCommandList() const{ return pDxCore_->GetCommandList(); }

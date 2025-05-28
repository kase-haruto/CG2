#pragma once

#include <d3d12.h>

struct PipelineSet{
	ID3D12PipelineState* pipelineState;
	ID3D12RootSignature* rootSignature;

	void SetCommand(ID3D12GraphicsCommandList* cmd) const{
		cmd->SetPipelineState(pipelineState);
		cmd->SetGraphicsRootSignature(rootSignature);
	}
};
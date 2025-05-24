#include "PipelineStateObject.h"

#include <Engine/Graphics/Context/GraphicsGroup.h>

void PipelineStateObject::SetRootSignature(ID3D12RootSignature* root) {
	rootSignature_ = root;
}

bool PipelineStateObject::Initialize([[maybe_unused]]const D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc) {
	//ID3D12Device* device = GraphicsGroup::GetInstance()->GetDevice().Get();

	//device->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&pipelineState_));

	//if (pipelineState_ == nullptr) {
	//	return false;
	//} else {
	//	return true;
	//}
	return true;
}
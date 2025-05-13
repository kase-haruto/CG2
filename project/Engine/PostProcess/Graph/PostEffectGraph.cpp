#include "PostEffectGraph.h"
#include <Engine/Graphics/RenderTarget/Interface/IRenderTarget.h>

void PostEffectGraph::AddPass(IPostEffectPass* pass){
	passes_.push_back(pass);
}

void PostEffectGraph::Execute(ID3D12GraphicsCommandList* cmd,
							  DxGpuResource* input,
							  IRenderTarget* finalTarget){
	input->Transition(cmd, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	D3D12_GPU_DESCRIPTOR_HANDLE currentInput = input->GetSRVGpuHandle();
	IRenderTarget* currentOutput = finalTarget;

	for (auto* pass : passes_){
		currentOutput->GetResource()->Transition(cmd, D3D12_RESOURCE_STATE_RENDER_TARGET);

		pass->Apply(cmd, currentInput, currentOutput);

		currentOutput->GetResource()->Transition(cmd, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

		currentInput = currentOutput->GetSRV();
	}

}

void PostEffectGraph::SetPassesFromList(const std::vector<PostEffectSlot>& slots) {
	passes_.clear();
	for (const auto& slot : slots) {
		if (slot.enabled && slot.pass) {
			passes_.push_back(slot.pass);
		}
	}
}
#include "ParticleRenderer.h"
#include <Engine/Graphics/Pipeline/Service/PipelineService.h>

#include <Engine/Application/Effects/Particle/Emitter/FxEmitter.h>
#include <Engine/Assets/Model/Modelmanager.h>

void ParticleRenderer::Initialize(ID3D12Device* device){
	instanceBuffer_.Initialize(device, 1024);
}

void ParticleRenderer::Render(const std::vector<FxEmitter*>& emitters,
							  PipelineService* pipelineService,
							  ID3D12GraphicsCommandList* cmdList){
	for (const auto* emitter : emitters){
		if (!emitter || emitter->GetUnits().empty()) continue;

		std::vector<ParticleConstantData> gpuUnits;
		for (const auto& fx : emitter->GetUnits()){
			if (!fx.alive) continue;
			gpuUnits.push_back({fx.position, fx.size, fx.color});
		}

		RenderUnits(emitter->GetModelPath(), gpuUnits, pipelineService, cmdList);
	}
}

void ParticleRenderer::RenderUnits(const std::string& modelPath,
							  const std::vector<ParticleConstantData>& gpuUnits,
							  PipelineService* pipelineService,
							  ID3D12GraphicsCommandList* cmdList){
	if (gpuUnits.empty()) return;

	ModelData& model = ModelManager::GetInstance()->GetModelData(modelPath);
	if (model.meshData.indices.empty()) return;

	auto device = GraphicsGroup::GetInstance()->GetDevice();

	if (!model.vertexBuffer.IsInitialized()){
		model.vertexBuffer.Initialize(device, static_cast< UINT >(model.meshData.vertices.size()));
		model.vertexBuffer.TransferVectorData(model.meshData.vertices);
	}
	if (!model.indexBuffer.IsInitialized()){
		model.indexBuffer.Initialize(device, static_cast< UINT >(model.meshData.indices.size()));
		model.indexBuffer.TransferVectorData(model.meshData.indices);
	}

	auto psoSet = pipelineService->GetPipelineSet(PipelineTag::Object::Particle, BlendMode::ADD);
	instanceBuffer_.TransferVectorData(gpuUnits);

	model.vertexBuffer.SetCommand(cmdList);
	model.indexBuffer.SetCommand(cmdList);
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	psoSet.SetCommand(cmdList);
	cmdList->DrawIndexedInstanced(static_cast< UINT >(model.meshData.indices.size()), static_cast< UINT >(gpuUnits.size()), 0, 0, 0);
}

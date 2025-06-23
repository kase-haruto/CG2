#include "ParticleRenderer.h"

#include <Engine/Application/Effects/Particle/Emitter/FxEmitter.h>
#include <Engine/Assets/Model/Modelmanager.h>
#include <Engine/Assets/Texture/TextureManager.h>
#include <Engine/Graphics/Pipeline/Service/PipelineService.h>
#include <Engine/Graphics/Camera/Manager/CameraManager.h>


void ParticleRenderer::Initialize(ID3D12Device* device) {
	instanceBuffer_.Initialize(device, 1024);
}

void ParticleRenderer::Render(const std::vector<FxEmitter*>& emitters,
							  PipelineService* pipelineService,
							  ID3D12GraphicsCommandList* cmdList) {
	auto psoSet = pipelineService->GetPipelineSet(PipelineTag::Object::Particle, BlendMode::ADD);
	psoSet.SetCommand(cmdList);

	CameraManager::GetInstance()->SetCommand(cmdList, PipelineType::StructuredObject);

	for (const auto* emitter : emitters) {
		if (!emitter || emitter->GetUnits().empty()) continue;

		emitter->GetMaterialBuffer().SetCommand(cmdList,1);

		auto textureHandle = TextureManager::GetInstance()->LoadTexture("Textures/" + emitter->GetTexturePath());
		cmdList->SetGraphicsRootDescriptorTable(3, textureHandle);

		std::vector<ParticleConstantData> gpuUnits;
		for (const auto& fx : emitter->GetUnits()) {
			if (fx.alive) {
				gpuUnits.push_back({ fx.position, fx.size, fx.color });
			}
		}

		RenderGrouped(emitter->GetModelPath(), gpuUnits, cmdList);
	}
}

void ParticleRenderer::RenderGrouped(const std::string& modelPath,
									 const std::vector<ParticleConstantData>& gpuUnits,
									 ID3D12GraphicsCommandList* cmdList) {
	if (gpuUnits.empty()) return;

	ModelData& model = ModelManager::GetInstance()->GetModelData(modelPath);
	if (model.meshData.indices.empty()) return;

	auto device = GraphicsGroup::GetInstance()->GetDevice();

	if (!model.vertexBuffer.IsInitialized()) {
		model.vertexBuffer.Initialize(device, static_cast<UINT>(model.meshData.vertices.size()));
		model.vertexBuffer.TransferVectorData(model.meshData.vertices);
	}
	if (!model.indexBuffer.IsInitialized()) {
		model.indexBuffer.Initialize(device, static_cast<UINT>(model.meshData.indices.size()));
		model.indexBuffer.TransferVectorData(model.meshData.indices);
	}

	instanceBuffer_.TransferVectorData(gpuUnits);
	instanceBuffer_.CreateSrv(device);
	model.vertexBuffer.SetCommand(cmdList);
	model.indexBuffer.SetCommand(cmdList);
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	cmdList->SetGraphicsRootDescriptorTable(2, instanceBuffer_.GetGpuHandle());
	cmdList->DrawInstanced(4, static_cast<UINT>(gpuUnits.size()), 0, 0);
}

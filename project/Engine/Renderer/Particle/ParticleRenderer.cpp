#include "ParticleRenderer.h"
#include <Engine/Graphics/Pipeline/Service/PipelineService.h>

#include <Engine/Assets/Model/Modelmanager.h>

void ParticleRenderer::Initialize(ID3D12Device* device) {
    instanceBuffer_.Initialize(device, 1024);
}

void ParticleRenderer::Render(ID3D12GraphicsCommandList* cmdList, const std::vector<FxUnit>& units, PipelineService* pipelineService) {
    std::unordered_map<std::string, std::vector<ParticleConstantData>> grouped;

    for (const auto& fx : units) {
        if (!fx.alive) continue;
        grouped[fx.modelPath].push_back({ fx.position, fx.size, fx.color });
    }

    for (auto& [modelPath, gpuUnits] : grouped) {
        ModelData& model = ModelManager::GetInstance()->GetModelData(modelPath);
        if (model.meshData.indices.empty()) continue;

        // モデルごとの初期化確認
        if (!initializedModels_[modelPath]) {
            auto device = GraphicsGroup::GetInstance()->GetDevice();

            if (model.vertexBuffer.GetCount() == 0) {
                model.vertexBuffer.Initialize(device, static_cast<UINT>(model.meshData.vertices.size()));
                model.vertexBuffer.TransferVectorData(model.meshData.vertices);
            }
            if (model.indexBuffer.GetCount() == 0) {
                model.indexBuffer.Initialize(device, static_cast<UINT>(model.meshData.indices.size()));
                model.indexBuffer.TransferVectorData(model.meshData.indices);
            }

            initializedModels_[modelPath] = true;
        }

        auto psoSet = pipelineService->GetPipelineSet(PipelineTag::Object::Particle, BlendMode::ADD);
        instanceBuffer_.TransferVectorData(gpuUnits);

        model.vertexBuffer.SetCommand(cmdList);
        model.indexBuffer.SetCommand(cmdList);
        cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        psoSet.SetCommand(cmdList);
        cmdList->DrawIndexedInstanced(static_cast<UINT>(model.meshData.indices.size()), static_cast<UINT>(gpuUnits.size()), 0, 0, 0);
    }
}

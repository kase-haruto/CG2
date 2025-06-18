#include "ParticleRenderer.h"

#include <Engine/Application/Effects/Particle/Emitter/FxEmitter.h>
#include <Engine/Assets/Model/Modelmanager.h>
#include <Engine/Assets/Texture/TextureManager.h>
#include <Engine/Graphics/Pipeline/Service/PipelineService.h>

void ParticleRenderer::Initialize(ID3D12Device* device){
	instanceBuffer_.Initialize(device, 1024);
}

void ParticleRenderer::Render(const std::vector<FxEmitter*>& emitters,
							  PipelineService* pipelineService,
							  ID3D12GraphicsCommandList* cmdList){
	// モデルごとにFxUnitをグルーピング
	struct BatchKey{
		std::string modelPath;
		std::string texturePath;
		ParticleMaterial material;

		bool operator==(const BatchKey& other) const{
			return modelPath == other.modelPath && texturePath == other.texturePath && memcmp(&material, &other.material, sizeof(Material)) == 0;
		}
	};

	struct BatchKeyHasher{
		std::size_t operator()(const BatchKey& key) const{
			std::size_t h1 = std::hash<std::string>()(key.modelPath);
			std::size_t h2 = std::hash<std::string>()(key.texturePath);
			std::size_t h3 = std::hash<size_t>()(reinterpret_cast< size_t >(key.material.color.x));
			return h1 ^ (h2 << 1) ^ (h3 << 2);
		}
	};

	std::unordered_map<BatchKey, std::vector<ParticleConstantData>, BatchKeyHasher> grouped;

	for (const auto* emitter : emitters){
		if (!emitter) continue;
		const std::string& modelPath = emitter->GetModelPath();
		const std::string& texturePath = emitter->GetTexturePath();
		const ParticleMaterial& material = emitter->GetMaterial();

		for (const auto& fx : emitter->GetUnits()){
			if (fx.alive){
				grouped[{modelPath, texturePath, material}].push_back({fx.position, fx.size, fx.color});
			}
		}
	}

	// パイプライン設定（共通）
	auto psoSet = pipelineService->GetPipelineSet(PipelineTag::Object::Particle, BlendMode::ADD);
	psoSet.SetCommand(cmdList);

	// 各モデルに対して描画
	for (auto& [key, gpuUnits] : grouped){
		RenderGrouped(key.modelPath, key.texturePath, key.material, gpuUnits, pipelineService, cmdList);
	}
}

void ParticleRenderer::RenderGrouped(const std::string& modelPath,
									 const std::string& texturePath,
									 const ParticleMaterial& material,
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

	instanceBuffer_.TransferVectorData(gpuUnits);

	// Material用CBV
	D3D12_GPU_VIRTUAL_ADDRESS materialAddress = GraphicsGroup::GetInstance()->UploadToMaterialHeap(&material, sizeof(ParticleMaterial));
	cmdList->SetGraphicsRootConstantBufferView(1, materialAddress);

	// Texture用SRV
	auto textureHandle = TextureManager::GetInstance()->LoadTexture("Textures/" + texturePath);
	cmdList->SetGraphicsRootDescriptorTable(3, textureHandle);

	// 描画コマンド発行
	model.vertexBuffer.SetCommand(cmdList);
	model.indexBuffer.SetCommand(cmdList);
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	cmdList->SetGraphicsRootDescriptorTable(2, instanceBuffer_.GetGpuHandle());
	cmdList->DrawIndexedInstanced(
		static_cast< UINT >(model.meshData.indices.size()),
		static_cast< UINT >(gpuUnits.size()),
		0, 0, 0);
}

#pragma once

#include <Engine/Application/Effects/Particle/FxUnit.h>
#include <Engine/Application/Effects/Particle/Detail/ParticleDetail.h>
#include <Engine/Graphics/Buffer/DxStructuredBuffer.h>

#include <unordered_map>
#include <string>
#include <memory>

class ParticleRenderer {
public:
	//===================================================================*/
	//					public functions
	//===================================================================*/
	void Initialize(ID3D12Device* device);

	void Render(const std::vector<class FxEmitter*>& emitters,
				class PipelineService* pipelineService,
				ID3D12GraphicsCommandList* cmdList);


private:
	//===================================================================*/
	//					private functions
	//===================================================================*/
	void RenderGrouped(const std::string& modelPath,
					   const std::string& texturePath,
					   const ParticleMaterial& material,
					   const std::vector<ParticleConstantData>& gpuUnits,
					   class PipelineService* pipelineService,
					   ID3D12GraphicsCommandList* cmdList);
private:
	//===================================================================*/
	//					private variables
	//===================================================================*/
	DxStructuredBuffer<ParticleConstantData> instanceBuffer_;
};
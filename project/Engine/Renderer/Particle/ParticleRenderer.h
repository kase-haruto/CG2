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
	void RegisterModelByPath(const std::string& modelPath);
	void Render(ID3D12GraphicsCommandList* cmdList, const std::vector<FxUnit>& units,class PipelineService* pipelineService);

private:
	//===================================================================*/
	//					private variables
	//===================================================================*/
	DxStructuredBuffer<ParticleConstantData> instanceBuffer_;
	std::unordered_map<std::string, std::shared_ptr<ModelData>> modelMap_;
};
#pragma once
/* ========================================================================
/* include space
/* ===================================================================== */
/* math */
#include <Engine/Foundation/Math/Vector3.h>
#include <Engine/Foundation/Math/Vector4.h>

/* engine */
#include <Engine/Graphics/Pipeline/PipelineType.h>
#include <Engine/Objects/3D/Actor/SceneObject.h>
#include <Engine/Graphics/Buffer/DxConstantBuffer.h>

/* config */
#include <Data/Engine/Configs/Scene/Objects/LightObjects/PointLightConfig.h>

/* lib */
#include <wrl.h>
#include <d3d12.h>

struct PointLightData{
	Vector4 color;		//ライトの色
	Vector3 position;	//ライトの位置
	float intensity;	//光度
	float radius;		//ライトの届く最大距離
	float decay;		//減衰率
	float pad[2];
};

class DxCore;

class PointLight
: public SceneObject{
public:
	PointLight(const std::string& name);
	PointLight() = default;
	~PointLight();

	void Initialize();
	void Update()override;
	void ShowGui()override;

	void SetCommand(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList, PipelineType type);
	void SaveConfig(const std::string& path)override;
	void LoadConfig(const std::string& path)override;

private:
	DxConstantBuffer<PointLightData> constantBuffer_;
	PointLightConfig config_;
};


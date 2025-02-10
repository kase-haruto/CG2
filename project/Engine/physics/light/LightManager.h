#pragma once

// engine 
#include "engine/graphics/Pipeline/PipelineType.h"
#include "LightData.h"
#include "PointLight.h"
#include "DirectionalLight.h"

// lib
#include <memory>
#include <wrl.h>
#include <d3d12.h>

// forward declaration
class DxCore;

class LightManager{
public:
	//===================================================================*/
	//                    singleton
	//===================================================================*/
	static LightManager* GetInstance();
	LightManager(const LightManager&) = delete;				//< コピーコンストラクタ削除
	LightManager& operator=(const LightManager&) = delete;	//< 代入演算子削除

public:
	//===================================================================*/
	//                   public functions
	//===================================================================*/
	void Initialize(const DxCore* dxCore);
	void Update();
	void Finalize();
	void ShowImGui();
	void SetCommand(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList,
					LightType lightType,
					PipelineType pipelineType);

private:
	//===================================================================*/
	//                    private functions
	//===================================================================*/
	LightManager(){};
	~LightManager() = default;

private:
	//===================================================================*/
	//                    private variables
	//===================================================================*/
	std::unique_ptr<DirectionalLight> directionalLight_;	//< ディレクショナルライト
	std::unique_ptr<PointLight> pointLight_;				//< ポイントライト

};


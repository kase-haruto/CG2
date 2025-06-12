#pragma once
/* ========================================================================
/* include space
/* ===================================================================== */
#include <Engine/objects/LightObject/DirectionalLight.h>
#include <Engine/objects/LightObject/PointLight.h>

#include <memory>

class LightLibrary{
public:
	//===================================================================*/
	//			public methods
	//===================================================================*/
	LightLibrary(class SceneObjectLibrary* library);
	LightLibrary() = default;
	~LightLibrary() = default;

	void Update();
	void Clear();
	void SetDirectionalLight(std::unique_ptr<DirectionalLight> light);
	void SetPointLight(std::unique_ptr<PointLight> light);

	DirectionalLight* GetDirectionalLight() const { return directionalLight_.get(); }
	PointLight* GetPointLight() const { return pointLight_.get(); }

	void SetCommand(ID3D12GraphicsCommandList* cmdList, PipelineType pipelineType);
	void SetCommand(ID3D12GraphicsCommandList* cmdList,
					PipelineType pipelineType,
					LightType lightType);
private:
	//===================================================================*/
	//			private methods
	//===================================================================*/
	std::unique_ptr<DirectionalLight> directionalLight_;
	std::unique_ptr<PointLight> pointLight_;

};


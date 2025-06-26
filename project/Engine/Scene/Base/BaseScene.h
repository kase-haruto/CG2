#pragma once
/* ========================================================================
/* include space
/* ===================================================================== */
// engine
#include <Engine/Scene/Base/IScene.h>
#include <Engine/Scene/Context/SceneContext.h>
#include <Engine/Extensions/SkyBox/SkyBox.h>

// c++
#include <string>

/* ========================================================================
/* シーン基底クラス
/* ===================================================================== */
class BaseScene :
	public IScene{
public:
	//===================================================================*/
	//			public methods
	//===================================================================*/
	BaseScene();
	~BaseScene() override = default;

	virtual void Initialize()override{}
	virtual void Update()override{}
	void Draw([[maybe_unused]] ID3D12GraphicsCommandList* cmdList, class PipelineService* psoService)override;
	void CleanUp()override{};
	virtual void LoadAssets()override {}
public:
	SceneContext* GetSceneContext() const override { return sceneContext_.get(); }
	void SetSceneName(const std::string& name){ sceneName_ = name; }
protected:
	//===================================================================*/
	//			protected methods
	//===================================================================*/
	std::unique_ptr<SceneContext> sceneContext_ = nullptr;
	std::unique_ptr<SkyBox> skyBox_ = nullptr;
	std::string sceneName_ = "Scene";
};


#pragma once
/* ========================================================================
/* include space
/* ===================================================================== */
// engine
#include <Engine/Scene/Base/IScene.h>
#include <Engine/Scene/Context/SceneContext.h>

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
	BaseScene() = default;
	BaseScene(DxCore* dxCore);
	~BaseScene() override = default;

	virtual void Initialize()override{}
	virtual void Update()override{}
	void Draw(ID3D12GraphicsCommandList* cmdList)override;
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

	std::string sceneName_ = "Scene";
};


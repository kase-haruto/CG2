#pragma once
/* ========================================================================
/* include space
/* ===================================================================== */
// engine
#include <Engine/scene/Interface/IScene.h>
#include <Engine/scene/Context/SceneContext.h>

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
	void Draw()override;
	void CleanUp()override{};
public:
	SceneContext* GetSceneContext() const { return sceneContext_.get(); }
	void SetSceneName(const std::string& name){ sceneName_ = name; }
protected:
	//===================================================================*/
	//			protected methods
	//===================================================================*/
	std::unique_ptr<SceneContext> sceneContext_ = nullptr;

	std::string sceneName_ = "Scene";
};


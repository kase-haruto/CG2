#pragma once
/* ========================================================================
/* include space
/* ===================================================================== */
// engine
#include <Engine/Application/UI/EngineUI/Core/EngineUICore.h>
#include <Engine/Objects/3D/Actor/SceneObject.h>
#include <Engine/Scene/Details/SceneType.h>

// lib
#include <vector>

// forward declaration
class DxCore;
class SceneManager;
class BaseCamera;

/* ========================================================================
/* シーンインターフェース
/* ===================================================================== */
class IScene{
public:
	//===================================================================*/
	//			public methods
	//===================================================================*/
	IScene();
	IScene(DxCore* dxCore);
	virtual ~IScene() = default;

	virtual void Initialize() = 0;
	virtual void Update() = 0;
	virtual void Draw(){}
	virtual void CleanUp() = 0;

	//--------- accessor -----------------------------------------------------
	void SetEngineUI(EngineUICore* ui){ pEngineUI_ = ui; }
	void SetSceneManager(SceneManager* sceneManager){ pSceneManager_ = sceneManager; }
	

protected:
	//===================================================================*/
	//			protected methods
	//===================================================================*/
	DxCore* pDxCore_ = nullptr;
	EngineUICore* pEngineUI_ = nullptr;
	SceneManager* pSceneManager_ = nullptr;
};

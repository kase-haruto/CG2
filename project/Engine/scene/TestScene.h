#pragma once
#include <Engine/scene/Base/BaseScene.h>
/* ========================================================================
/* include space
/* ===================================================================== */
#include "engine/objects/Sprite.h"
#include "Game/3d/Base/BaseGameObject.h"
#include "engine/physics/Triangle.h"
#include "engine/graphics/ViewProjection.h"
#include "engine/objects/ModelBuilder.h"
#include "engine/Editor/UiEditor.h"
#include "Engine/objects/Animation/AnimationModel.h"

/* graphics */
#include "engine/physics/light/DirectionalLight.h"
#include "engine/physics/light/PointLight.h"
#include "engine/graphics/FogEffect.h"

/* object */
#include "lib/myFunc/PrimitiveDrawer.h"
#include "Engine/objects/particle/DemoParticle.h"
#include "lib/myMath/Quaternion.h"

/* c++ */
#include <vector>
#include <memory>

///デバッグ関連///
#ifdef _DEBUG
#include"lib/ImGuiManager.h"
#include"externals/imgui/imgui.h"
#endif // _DEBUG

/* ========================================================================
/* testScene
/* ===================================================================== */
class TestScene final :
	public BaseScene{
public:
	//===================================================================*/
	//			public methods
	//===================================================================*/
	TestScene();
	TestScene(DxCore* dxCore);
	~TestScene() override = default;

	void Initialize()override;
	void Update()override;
	void CleanUp()override;

private:
	/* graphics =====================================================*/
	std::unique_ptr<FogEffect>fog_ = nullptr;

	/* objects ====================================================*/
	std::unique_ptr<Model> modelField_ = nullptr;
	std::unique_ptr<BaseGameObject> bunny_ = nullptr;
	std::unique_ptr<BaseGameObject> teapot_ = nullptr;
	std::unique_ptr<BaseGameObject> walkHuman_ = nullptr;

	//テスト用
	std::vector<std::unique_ptr<BaseGameObject>> humans_;

	/* editor =====================================================*/
	std::unique_ptr<ModelBuilder>modelBuilder_ = nullptr;
	std::unique_ptr<UIEditor> uiEditor_ = nullptr;

};


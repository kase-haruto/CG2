#pragma once
/* ========================================================================
/* include space
/* ===================================================================== */
/* engine */
#include <Engine/scene/Base/BaseScene.h>
#include <Engine/Renderer/Sprite/Sprite.h>
#include <Game/3d/Base/BaseGameObject.h>
#include <Engine/objects/SkyBox/SkyBox.h>

/* graphics */
#include <Engine/physics/light/DirectionalLight.h>
#include <Engine/physics/light/PointLight.h>
#include <Engine/Graphics/Effect/FogEffect.h>

/* object */
#include <Engine/Renderer/Primitive/PrimitiveDrawer.h>
#include <lib/myMath/Quaternion.h>

/* c++ */
#include <vector>
#include <memory>

///デバッグ関連///
#ifdef _DEBUG
#include <Engine/Application/UI/ImGuiManager.h>
#include <externals/imgui/imgui.h>
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

	std::unique_ptr<SkyBox> skyBox_ = nullptr;

	//テスト用
	std::vector<std::unique_ptr<BaseGameObject>> humans_;

};


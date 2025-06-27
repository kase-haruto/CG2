#pragma once
/* ========================================================================
/* include space
/* ===================================================================== */
/* engine */
#include <Engine/scene/Base/BaseScene.h>
#include <Engine/Renderer/Sprite/Sprite.h>
#include <Engine/Objects/3D/Actor/BaseGameObject.h>
#include <Engine/Extensions/SkyBox/SkyBox.h>
#include <Engine/Extensions/Fog/FogEffect.h>

/* graphics */
#include <Engine/Objects/LightObject/DirectionalLight.h>
#include <Engine/Objects/LightObject/PointLight.h>

/* object */
#include <Engine/Renderer/Primitive/PrimitiveDrawer.h>
#include <Engine/Foundation/Math/Quaternion.h>
#include <Engine/Application/Effects/Particle/Object/ParticleSystemObject.h>

/* c++ */
#include <vector>
#include <memory>

///デバッグ関連///
#ifdef _DEBUG

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
	~TestScene() override = default;

	void Initialize()override;
	void Update()override;
	void CleanUp()override;
	void LoadAssets()override;
private:
	/* graphics =====================================================*/
	std::unique_ptr<FogEffect>fog_ = nullptr;

	/* objects ====================================================*/

	//テスト用
	std::vector<std::unique_ptr<BaseGameObject>> humans_;

};


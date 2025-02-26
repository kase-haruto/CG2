#pragma once
#include "IScene.h"

///オブジェクト関連///
#include "engine/objects/Sprite.h"
#include "Game/3d/Base/BaseGameObject.h"
#include "engine/physics/Sphere.h"
#include "engine/physics/Triangle.h"
#include "engine/graphics/ViewProjection.h"
#include "Engine/objects/Animation/AnimationModel.h"

/* editors */
#include "Engine/objects/ModelBuilder.h"
#include "Engine/Editor/UiEditor.h"
#include "Engine/Editor/ParticleEditor.h"

/* graphics */
#include "engine/physics/light/DirectionalLight.h"
#include "engine/physics/light/PointLight.h"
#include "engine/graphics/FogEffect.h"

/* object */
#include "lib/myFunc/PrimitiveDrawer.h"
#include "Engine/objects/particle/DemoParticle.h"
#include "Engine/objects/particle/TornadoParticle.h"
#include "lib/myMath/Quaternion.h"

/* c++ */
#include <vector>
#include <memory>

///デバッグ関連///
#ifdef _DEBUG
#include"lib/ImGuiManager.h"
#include"externals/imgui/imgui.h"
#endif // _DEBUG



/// <summary>
/// テストシーン
/// </summary>
class TestScene final :
    public IScene{
public:
    TestScene();
    TestScene(DxCore* dxCore);
    ~TestScene() override = default;

    void Initialize()override;
    void Update()override;
    void Draw()override;
    void CleanUp()override;

	void Draw3dObject()override;

private:
	/* graphics =====================================================*/
    std::unique_ptr<FogEffect>fog_ = nullptr;

	/* objects ====================================================*/
    std::unique_ptr<Model> modelField_ = nullptr;
	std::unique_ptr<BaseGameObject> bunny_ = nullptr;
	std::unique_ptr<BaseGameObject> teapot_ = nullptr;

	/* editor =====================================================*/
    std::unique_ptr<ModelBuilder>modelBuilder_ = nullptr;
    std::unique_ptr<UIEditor> uiEditor_ = nullptr;
	std::unique_ptr<ParticleEditor> particleEditor_ = nullptr;
};


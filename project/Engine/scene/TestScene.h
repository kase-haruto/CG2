#pragma once
#include "IScene.h"

///オブジェクト関連///
#include "engine/objects/Sprite.h"
#include "engine/physics/DirectionalLight.h"
#include "engine/objects/Model.h"
#include "engine/physics/Sphere.h"
#include "engine/physics/Triangle.h"
#include "engine/graphics/ViewProjection.h"
#include "engine/objects/ModelBuilder.h"
#include "engine/objects/Particle.h"
#include "engine/Editor/UiEditor.h"
#include "engine/objects/Particle.h"

/* graphics */
#include "engine/physics/DirectionalLight.h"
#include "engine/physics/PointLight.h"
#include "engine/graphics/FogEffect.h"

/* object */
#include "lib/myFunc/PrimitiveDrawer.h"

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

    /// <summary>
    /// 初期化処理
    /// </summary>
    void Initialize()override;
    /// <summary>
    /// 更新処理
    /// </summary>
    void Update()override;
    /// <summary>
    /// デバッグ用ui
    /// </summary>
    void UpdateDebugUI();
    /// <summary>
    /// 描画処理
    /// </summary>
    void Draw()override;
    /// <summary>
    /// 解放処理
    /// </summary>
    void Finalize()override;

    /// <summary>
    /// モデル描画前処理
    /// </summary>
    void ModelPreDraw();
private:
    ///=========================
    /// グラフィック関連
    ///=========================
    std::unique_ptr<DirectionalLight>directionalLight_ = nullptr;
    std::unique_ptr<PointLight> pointLight_ = nullptr;
    std::unique_ptr<FogEffect>fog_ = nullptr;

    ///=========================
    /// カメラ関連
    ///=========================

    ///=========================
    /// オブジェクト関連
    ///=========================
    std::unique_ptr<ModelBuilder>modelBuilder_ = nullptr;
    std::unique_ptr<UIEditor> uiEditor_ = nullptr;
    std::unique_ptr<Model> modelField_ = nullptr;
    std::unique_ptr<Sphere>sphere_ = nullptr;
    std::unique_ptr<Model> modelGround_ = nullptr;

    ///=========================
    /// particle
    ///=========================
    std::unique_ptr<ParticleManager>particle_ = nullptr;
    
    // 各ウィンドウの表示フラグ
    bool showObjectWindow = false;

};


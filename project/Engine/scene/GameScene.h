#pragma once

#include "IScene.h"

/* objects */
#include "engine/objects/ModelBuilder.h"
#include "engine/Editor/UiEditor.h"
#include "Game/3d/Player/Player.h"
#include "Game/3d/Player/PlayerAttack/PlayerAttackEditor.h"
#include "Game/3d/Enemy/Enemy.h"
#include "Engine/core/UI/AttackEditorPanel.h"

/* graphics */
#include "engine/physics/DirectionalLight.h"
#include "engine/physics/PointLight.h"
#include "engine/graphics/FogEffect.h"

/* object */
#include "lib/myFunc/PrimitiveDrawer.h"
#include "Engine/objects/particle/DemoParticle.h"
#include "Engine/objects/particle/TornadoParticle.h"

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
class GameScene final :
    public IScene{
public:
    GameScene();
    GameScene(DxCore* dxCore);
    ~GameScene() override = default;

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
    void CleanUp()override;

    /// <summary>
    /// モデル描画前処理
    /// </summary>
    void ModelPreDraw();
private:
    std::unique_ptr<DirectionalLight>directionalLight_ = nullptr;
    std::unique_ptr<PointLight> pointLight_ = nullptr;
    std::unique_ptr<FogEffect>fog_ = nullptr;

    /* objects ====================================================*/
    std::unique_ptr<ModelBuilder>modelBuilder_ = nullptr;
    std::unique_ptr<UIEditor> uiEditor_ = nullptr;
    std::unique_ptr<Model> modelField_ = nullptr;
    std::unique_ptr<Model> modelGround_ = nullptr;
    std::unique_ptr<Player> player_ = nullptr;
	std::unique_ptr<PlayerAttackEditor> playerAttackEditor_ = nullptr;
    std::unique_ptr<Enemy> enemy_ = nullptr;

	std::unique_ptr<AttackEditorPanel> attackEditorPanel_ = nullptr;

    // 各ウィンドウの表示フラグ
    bool showObjectWindow = false;

};


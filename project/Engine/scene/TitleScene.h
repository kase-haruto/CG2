#pragma once
#include "IScene.h"

#include "engine/objects/ModelBuilder.h"
#include "Engine/objects/Sprite.h"
#include "Engine/graphics/FogEffect.h"
#include "Engine/physics/light/DirectionalLight.h"
#include "Engine/physics/light/PointLight.h"

#include <memory>

class TitleScene :
    public IScene{
public:
    TitleScene() = default;
    TitleScene(DxCore* dxCore);
    ~TitleScene()override = default;

    void Initialize() override;      // 初期化処理
    void Update() override;          // 更新処理 
    void Draw() override;            // 描画処理
    void CleanUp() override;        // 終了処理
    void ModelPreDraw() override;	// モデル描画前処理

private:
    //===================================================================*/
    //                   private fields
    //===================================================================*/

    //* グラフィック関連 *//
    std::unique_ptr<DirectionalLight>directionalLight_ = nullptr;
    std::unique_ptr<PointLight> pointLight_ = nullptr;
    std::unique_ptr<FogEffect>fog_ = nullptr;

    std::unique_ptr<Model> modelField_ = nullptr;
	std::unique_ptr<Sprite> titleSprite_ = nullptr;
	std::unique_ptr<Sprite> startSprite_ = nullptr;
};

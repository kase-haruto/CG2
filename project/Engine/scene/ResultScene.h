#pragma once
#include "IScene.h"

#include "Engine/graphics/FogEffect.h"
#include "Engine/physics/DirectionalLight.h"
#include "Engine/physics/PointLight.h"

#include <memory>

class ResultScene :
    public IScene{
public:
    ResultScene() = default;
    ResultScene(DxCore* dxCore);
    ~ResultScene()override = default;

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
};

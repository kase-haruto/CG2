#pragma once
#include "IScene.h"

///オブジェクト関連///
#include"Sprite.h"
#include"DirectionalLight.h"
#include"DirectXCommon.h"
#include"Model.h"
#include"Sphere.h"
#include"Triangle.h"
#include"ViewProjection.h"

///デバッグ関連///
#ifdef _DEBUG
#include"ImGuiManager.h"
#include"imgui.h"
#endif // _DEBUG


#include<memory>

/// <summary>
/// テストシーン
/// </summary>
class TestScene final :
    public IScene{
public:
    TestScene();
    ~TestScene() override;

    /// <summary>
    /// 初期化処理
    /// </summary>
    void Initialize()override;
    /// <summary>
    /// 更新処理
    /// </summary>
    void Update()override;
    /// <summary>
    /// 描画処理
    /// </summary>
    void Draw()override;
    /// <summary>
    /// 解放処理
    /// </summary>
    void Finalize()override;
private:
    ///=========================
    /// グラフィック関連
    ///=========================
    DirectXCommon* dxCommon_;

    ///=========================
    /// カメラ関連
    ///=========================
    std::unique_ptr<ViewProjection> viewProjection_;
    
    ///=========================
    /// オブジェクト関連
    ///=========================
    std::unique_ptr<Sprite>sprite_;
    std::unique_ptr<DirectionalLight>light_;
    std::unique_ptr<Model>model_;
    std::unique_ptr<Sphere>sphere_;
    std::unique_ptr<Triangle>triangle_;
};


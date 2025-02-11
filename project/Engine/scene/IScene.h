#pragma once

#include "Engine/core/EngineUI.h"
#include "Engine/objects/SceneObject.h"
// lib
#include <string>
#include <vector>

class DxCore;
class SceneManager;

enum class SceneType{
    PLAY,
    TEST,
    count,
};

//===================================================================*/
// ISceneクラス(インターフェイス)
// 各シーンはこのクラスを継承して実装する
//===================================================================*/
class IScene{
public:
    IScene();
    IScene(DxCore* dxCore);
    virtual ~IScene() = default;

    // 初期化処理
    virtual void Initialize() = 0;
    // 更新処理
    virtual void Update() = 0;
    // 描画処理
    virtual void Draw() = 0;
    // 終了処理
    virtual void CleanUp() = 0;

    // モデル描画前処理(必要時オーバーライド)
    virtual void ModelPreDraw(){}

    // UIとシーンマネージャのセット
    void SetEngineUI(EngineUI* ui){ pEngineUI_ = ui; }
    void SetSceneManager(SceneManager* sceneManager){ pSceneManager_ = sceneManager; }
    void SetSceneName(const std::string& name){ sceneName_ = name; }

protected:
    DxCore* pDxCore_ = nullptr;
    EngineUI* pEngineUI_ = nullptr;
    SceneManager* pSceneManager_ = nullptr;
    std::string    sceneName_;
};

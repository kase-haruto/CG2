#pragma once

// engine
#include "ModelData.h"
#include "graphics/GraphicsGroup.h"
#include "myFunc/MyFunc.h"

// lib
#include "d3d12.h"

#include <unordered_map>
#include <string>
#include <memory>
#include <wrl.h>

class ModelManager{
public:
    // Singleton インスタンスの取得
    static ModelManager* GetInstance();

    static void Initialize();

    /// <summary>
    /// モデルデータ取得
    /// </summary>
    /// <param name="fileName"></param>
    /// <returns></returns>
    static std::shared_ptr<ModelData> GetModelData(const std::string& fileName);

    /// <summary>
    /// モデルの読み込み
    /// </summary>
    /// <param name="fileName"></param>
    static std::shared_ptr<ModelData> LoadModel(const std::string& fileName);

    /// <summary>
    /// 解放処理
    /// </summary>
    void Finalize();

private:
    /// <summary>
    /// コンストラクタ/デストラクタ
    /// </summary>
    ModelManager() = default;
    ~ModelManager() = default;

private:
    // staticにするためのインスタンス
    static ModelManager* instance_;

    // ディレクトリパス
    static const std::string directoryPath_;

    // モデルデータ
    std::unordered_map<std::string, std::shared_ptr<ModelData>> modelDatas_;
};

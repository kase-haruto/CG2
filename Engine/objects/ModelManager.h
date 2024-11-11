#pragma once

// engine
#include "../objects/ModelData.h"
#include "engine/graphics/GraphicsGroup.h"
#include "lib/myFunc/MyFunc.h"

// lib
#include <d3d12.h>
#include <unordered_map>
#include <string>
#include <memory>
#include <wrl.h>

class ModelManager{
public:
    static ModelManager* GetInstance();
    static void Initialize();

    /// <summary>
    /// モデルデータ取得
    /// </summary>
    static std::shared_ptr<ModelData> GetModelData(const std::string& fileName);

    /// <summary>
    /// モデルの読み込み
    /// </summary>
    static std::shared_ptr<ModelData> LoadModel(const std::string& fileName);

    /// <summary>
    /// 頂点リソースの取得
    /// </summary>
    static Microsoft::WRL::ComPtr<ID3D12Resource> GetVertexResource(const std::string& fileName);

    /// <summary>
    /// インデックスリソースの取得
    /// </summary>
    static Microsoft::WRL::ComPtr<ID3D12Resource> GetIndexResource(const std::string& fileName);

    /// <summary>
    /// 初期化時読み込み
    /// </summary>
    static void StartUpLoad();

    /// <summary>
    /// 解放処理
    /// </summary>
    void Finalize();

private:
    ModelManager() = default;
    ~ModelManager() = default;

private:
    static ModelManager* instance_;
    static const std::string directoryPath_;
    std::unordered_map<std::string, std::shared_ptr<ModelData>> modelDatas_;

    // 頂点・インデックスバッファを保存するマップ
    std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D12Resource>> vertexBuffers_;
    std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D12Resource>> indexBuffers_;
};

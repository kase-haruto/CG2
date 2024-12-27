#pragma once

// engine
#include "engine/objects/ModelData.h"
#include "engine/graphics/GraphicsGroup.h"
#include "lib/myFunc/MyFunc.h"
#include "Engine/objects/Animation/AnimationStruct.h"

// lib
#include "lib/myMath/Quaternion.h"

#include <d3d12.h>
#include <unordered_map>
#include <string>
#include <memory>
#include <wrl.h>

#include<assimp/Importer.hpp>
#include<assimp/scene.h>
#include<assimp/postprocess.h>

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

    // ロードされたモデル名のリストを取得
    std::vector<std::string> GetLoadedModelNames() const;

private:
    ModelData LoadModelFile(const std::string& directoryPath, const std::string& fileNameWithExt);

    void LoadMesh(const aiMesh* mesh, ModelData& modelData);

    void LoadMaterial(const aiScene* scene, const aiMesh* mesh, ModelData& modelData);

    void LoadUVTransform(const aiMaterial* material, MaterialData& outMaterial);


    //指定時間に対して補完結果を返す
	Vector3 Evaluate(const AnimationCurve<Vector3>& curve, float time);
	Quaternion Evaluate(const AnimationCurve<Quaternion>& curve, float time);

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
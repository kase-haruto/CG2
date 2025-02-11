#pragma once

#include "Engine/objects/ModelData.h"
#include "Engine/graphics/GraphicsGroup.h"
#include "Engine/objects/Animation/AnimationStruct.h"
#include "lib/myMath/Quaternion.h"
#include "lib/myFunc/MyFunc.h"

// Assimp
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

// DX12
#include <d3d12.h>
#include <wrl.h>

// STL
#include <unordered_map>
#include <string>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <queue>
#include <future>
#include <functional>

class ModelManager{
public:
    static ModelManager* GetInstance();
    static void Initialize();
    static void Finalize();

    /// <summary>
    /// 非同期でモデルをロードする (ワーカースレッド1本で順番に処理)
    /// </summary>
    /// <param name="fileName">モデルのファイル名(例: "suzanne.obj")</param>
    /// <returns>ロード完了時の ModelData を指す future</returns>
    static std::future<std::shared_ptr<ModelData>> LoadModel(const std::string& fileName);

    /// <summary>
    /// ロードが完了したモデルの GPUリソース作成をまとめて行う
    /// (フレーム更新時などメインスレッドで呼ぶ想定)
    /// </summary>
    void ProcessLoadingTasks();

    /// <summary>
    /// ロード済みのモデルを取得(非同期ロード中の場合は nullptr の可能性もある)
    /// </summary>
    std::shared_ptr<ModelData> GetModelData(const std::string& fileName);

    /// <summary>
    /// ロード完了時に呼ばれるコールバックを設定する
    /// </summary>
    void SetOnModelLoadedCallback(std::function<void(const std::string&)> callback);

    /// <summary>
    /// サンプル: 複数モデルを一括でロード
    /// </summary>
    static void StartUpLoad();

    /// <summary>
    /// ロード済みモデルのファイル名一覧を取得
    /// </summary>
    std::vector<std::string> GetLoadedModelNames() const;

private:
    ModelManager();
    ~ModelManager();

    /// ファイルを読み込み→CPU側でModelDataを構築する (Assimp使用)
    ModelData LoadModelFile(const std::string& directoryPath, const std::string& fileNameWithExt);

    /// 頂点/インデックスバッファを GPU上に作成し、ModelData に登録
    void CreateGpuResources(const std::string& fileName, std::shared_ptr<ModelData> model);

    // メッシュやマテリアルなどの細かい読み込み
    void LoadMesh(const aiMesh* mesh, ModelData& modelData);
    void LoadMaterial(const aiScene* scene, const aiMesh* mesh, ModelData& modelData);
    void LoadUVTransform(const aiMaterial* material, MaterialData& outMaterial);

    // アニメーション評価関連
    Vector3 Evaluate(const AnimationCurve<Vector3>& curve, float time);
    Quaternion Evaluate(const AnimationCurve<Quaternion>& curve, float time);

private:
    // シングルトン
    static ModelManager* instance_;
    static const std::string directoryPath_;

    // ------------------------------------------------------
    // モデルデータマップ (ファイル名 -> ModelData)
    // ------------------------------------------------------
    std::unordered_map<std::string, std::shared_ptr<ModelData>> modelDatas_;
    mutable std::mutex modelDataMutex_;

    // ------------------------------------------------------
    // ワーカースレッド (1本)
    // ------------------------------------------------------
    std::thread workerThread_;
    bool stopWorker_ = false;
    std::mutex taskQueueMutex_;
    std::condition_variable taskQueueCv_;

    // ロードリクエスト
    struct LoadRequest{
        std::string fileName;
        std::promise<std::shared_ptr<ModelData>> promise;
    };
    std::queue<LoadRequest> requestQueue_; // リクエスト待ち行列

    // ------------------------------------------------------
    // CPUロード完了後のタスク (GPUリソース化待ち)
    // ------------------------------------------------------
    struct LoadingTask{
        std::string fileName;
        std::shared_ptr<ModelData> modelData;
    };
    std::vector<LoadingTask> pendingTasks_;
    std::mutex pendingTasksMutex_;

    // ロード完了コールバック
    std::function<void(const std::string&)> onModelLoadedCallback_;

    // ------------------------------------------------------
    // ワーカースレッド本体処理
    // ------------------------------------------------------
    void WorkerMain();
};

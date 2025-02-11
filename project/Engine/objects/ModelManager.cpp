#include "ModelManager.h"

// static 変数初期化
ModelManager* ModelManager::instance_ = nullptr;
const std::string ModelManager::directoryPath_ = "Resources/models";

ModelManager::ModelManager(){
    // スレッドを起動
    workerThread_ = std::thread(&ModelManager::WorkerMain, this);
}

ModelManager::~ModelManager(){
    // ワーカー終了指示
    {
        std::lock_guard<std::mutex> lock(taskQueueMutex_);
        stopWorker_ = true;
    }
    taskQueueCv_.notify_all();

    if (workerThread_.joinable()){
        workerThread_.join();
    }
}

ModelManager* ModelManager::GetInstance(){
    if (!instance_){
        instance_ = new ModelManager();
    }
    return instance_;
}

void ModelManager::Initialize(){
    GetInstance();
}

void ModelManager::Finalize(){
    if (instance_){
        delete instance_;
        instance_ = nullptr;
    }
}

//----------------------------------------------------------------------------
// 非同期ロード開始
//----------------------------------------------------------------------------
std::future<std::shared_ptr<ModelData>> ModelManager::LoadModel(const std::string& fileName){
    // 既にロード済みかどうかチェック
    {
        std::lock_guard<std::mutex> lock(instance_->modelDataMutex_);
        auto it = instance_->modelDatas_.find(fileName);
        if (it != instance_->modelDatas_.end()){
            // 既にロード済みなら、即座に value を設定した future を返す
            std::promise<std::shared_ptr<ModelData>> promise;
            promise.set_value(it->second);
            return promise.get_future();
        }
    }

    // キューに新しいリクエストを積む
    LoadRequest request;
    request.fileName = fileName;
    std::future<std::shared_ptr<ModelData>> fut = request.promise.get_future();

    {
        std::lock_guard<std::mutex> lock(instance_->taskQueueMutex_);
        instance_->requestQueue_.push(std::move(request));
    }
    instance_->taskQueueCv_.notify_one();

    return fut;
}

//----------------------------------------------------------------------------
// ワーカースレッドのループ処理
//----------------------------------------------------------------------------
void ModelManager::WorkerMain(){
    while (true){
        LoadRequest currentRequest;
        {
            // リクエスト待ち
            std::unique_lock<std::mutex> lock(taskQueueMutex_);
            taskQueueCv_.wait(lock, [this]{
                return stopWorker_ || !requestQueue_.empty();
                              });
            if (stopWorker_ && requestQueue_.empty()){
                // 終了指示
                return;
            }
            // キュー先頭を取り出し
            currentRequest = std::move(requestQueue_.front());
            requestQueue_.pop();
        }

        // ファイルを読み込み
        std::shared_ptr<ModelData> newModel = std::make_shared<ModelData>(
            LoadModelFile(directoryPath_, currentRequest.fileName)
        );

        // (B) GPUリソース作成はメインスレッドで行うため、一旦 pendingTasks_ に格納
        {
            std::lock_guard<std::mutex> lock(pendingTasksMutex_);
            pendingTasks_.push_back({currentRequest.fileName, newModel});
        }

        // CPUロード完了を promise で通知
        currentRequest.promise.set_value(newModel);
    }
}

//----------------------------------------------------------------------------
// (メインスレッド) CPUロードが終わったタスクを取り出して GPUリソースを作成
//----------------------------------------------------------------------------
void ModelManager::ProcessLoadingTasks(){
    std::vector<LoadingTask> tasks;
    {
        std::lock_guard<std::mutex> lock(pendingTasksMutex_);
        tasks.swap(pendingTasks_);
    }

    for (auto& t : tasks){
        // GPUリソースを生成
        CreateGpuResources(t.fileName, t.modelData);

        // modelDatas_ に登録
        {
            std::lock_guard<std::mutex> lock(modelDataMutex_);
            modelDatas_[t.fileName] = t.modelData;
        }

        // コールバック呼び出し(任意)
        if (onModelLoadedCallback_){
            onModelLoadedCallback_(t.fileName);
        }
    }
}

//----------------------------------------------------------------------------
// ロード済みモデルを取得（まだロード中なら nullptr）
//----------------------------------------------------------------------------
std::shared_ptr<ModelData> ModelManager::GetModelData(const std::string& fileName){
    std::lock_guard<std::mutex> lock(modelDataMutex_);
    auto it = modelDatas_.find(fileName);
    if (it != modelDatas_.end()){
        return it->second;
    }
    return nullptr;
}

//----------------------------------------------------------------------------
// ロード完了時のコールバック設定
//----------------------------------------------------------------------------
void ModelManager::SetOnModelLoadedCallback(std::function<void(const std::string&)> callback){
    onModelLoadedCallback_ = callback;
}

//----------------------------------------------------------------------------
// 複数ファイルをまとめてロード (サンプル)
//----------------------------------------------------------------------------
void ModelManager::StartUpLoad(){
    LoadModel("suzanne.obj");
    LoadModel("debugCube.obj");
    LoadModel("fence.obj");
    LoadModel("ground.obj");
    LoadModel("multiMaterial.obj");
    LoadModel("multiMesh.obj");
    LoadModel("plane.obj");
    LoadModel("teapot.obj");
    LoadModel("terrain.obj");
    LoadModel("bunny.obj");
}

//----------------------------------------------------------------------------
// ロード済みモデル名の一覧を返す
//----------------------------------------------------------------------------
std::vector<std::string> ModelManager::GetLoadedModelNames() const{
    std::lock_guard<std::mutex> lock(modelDataMutex_);
    std::vector<std::string> result;
    for (auto& kv : modelDatas_){
        result.push_back(kv.first);
    }
    return result;
}

//=============================================================================
//  下記はファイル読み込みやGPUリソース作成の実装詳細
//=============================================================================
ModelData ModelManager::LoadModelFile(const std::string& directoryPath, const std::string& fileNameWithExt){
    Assimp::Importer importer;

    // パスを組み立て
    std::string filePath = directoryPath + "/"
        + fileNameWithExt.substr(0, fileNameWithExt.find_last_of('.')) + "/"
        + fileNameWithExt;

    const aiScene* scene = importer.ReadFile(
        filePath.c_str(),
        aiProcess_Triangulate |
        aiProcess_FlipUVs |
        aiProcess_CalcTangentSpace
    );
    if (!scene){
        throw std::runtime_error("Assimp failed to load: " + filePath);
    }
    if (!scene->HasMeshes()){
        throw std::runtime_error("No meshes found in file: " + filePath);
    }

    ModelData modelData;

    // メッシュデータを格納
    for (unsigned int meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex){
        const aiMesh* mesh = scene->mMeshes[meshIndex];
        LoadMesh(mesh, modelData);
        LoadMaterial(scene, mesh, modelData);
    }

    // アニメーション(サンプル)
    if (scene->HasAnimations()){
        aiAnimation* aiAnim = scene->mAnimations[0];
        Animation animation;
        float ticksPerSecond = ( float ) (aiAnim->mTicksPerSecond != 0 ? aiAnim->mTicksPerSecond : 25.0f);
        animation.duration = ( float ) (aiAnim->mDuration / ticksPerSecond);

        for (unsigned int channelIdx = 0; channelIdx < aiAnim->mNumChannels; ++channelIdx){
            aiNodeAnim* nodeAnim = aiAnim->mChannels[channelIdx];
            NodeAnimation nodeAnimation;

            // Translation
            for (unsigned int i = 0; i < nodeAnim->mNumPositionKeys; ++i){
                float time = ( float ) (nodeAnim->mPositionKeys[i].mTime / ticksPerSecond);
                aiVector3D aiPos = nodeAnim->mPositionKeys[i].mValue;
                nodeAnimation.translate.keyframes.push_back({time, Vector3{aiPos.x, aiPos.y, aiPos.z}});
            }
            // Rotation
            for (unsigned int i = 0; i < nodeAnim->mNumRotationKeys; ++i){
                float time = ( float ) (nodeAnim->mRotationKeys[i].mTime / ticksPerSecond);
                aiQuaternion aiRot = nodeAnim->mRotationKeys[i].mValue;
                nodeAnimation.rotate.keyframes.push_back({time, Quaternion{aiRot.x, aiRot.y, aiRot.z, aiRot.w}});
            }
            // Scaling
            for (unsigned int i = 0; i < nodeAnim->mNumScalingKeys; ++i){
                float time = ( float ) (nodeAnim->mScalingKeys[i].mTime / ticksPerSecond);
                aiVector3D aiScale = nodeAnim->mScalingKeys[i].mValue;
                nodeAnimation.scale.keyframes.push_back({time, Vector3{aiScale.x, aiScale.y, aiScale.z}});
            }
            std::string nodeName(nodeAnim->mNodeName.C_Str());
            animation.nodeAnimations[nodeName] = nodeAnimation;
        }
        modelData.animation = animation;
    }

    return modelData;
}

//----------------------------------------------------------------------------
// GPUリソース作成 (ProcessLoadingTasksから呼ばれる)
//----------------------------------------------------------------------------
void ModelManager::CreateGpuResources([[maybe_unused]]const std::string& fileName, std::shared_ptr<ModelData> model){
    auto device = GraphicsGroup::GetInstance()->GetDevice();

    size_t vertexBufferSize = sizeof(VertexData) * model->vertices.size();
    auto vertexBuffer = CreateBufferResource(device, vertexBufferSize);
    {
        VertexData* mapped = nullptr;
        vertexBuffer->Map(0, nullptr, reinterpret_cast< void** >(&mapped));
        memcpy(mapped, model->vertices.data(), vertexBufferSize);
        vertexBuffer->Unmap(0, nullptr);
    }

    size_t indexBufferSize = sizeof(uint32_t) * model->indices.size();
    auto indexBuffer = CreateBufferResource(device, indexBufferSize);
    {
        uint32_t* mapped = nullptr;
        indexBuffer->Map(0, nullptr, reinterpret_cast< void** >(&mapped));
        memcpy(mapped, model->indices.data(), indexBufferSize);
        indexBuffer->Unmap(0, nullptr);
    }

    // ModelData にバッファリソースをセット
    model->vertexBufferResource = vertexBuffer;
    model->indexBufferResource = indexBuffer;
}

//----------------------------------------------------------------------------
// メッシュ読み込み
//----------------------------------------------------------------------------
void ModelManager::LoadMesh(const aiMesh* mesh, ModelData& modelData){
    for (unsigned int i = 0; i < mesh->mNumVertices; ++i){
        VertexData vertex {};
        vertex.position = {mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z, 1.0f};
        if (mesh->HasNormals()){
            vertex.normal = {mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z};
        }
        if (mesh->HasTextureCoords(0)){
            vertex.texcoord.x = mesh->mTextureCoords[0][i].x;
            vertex.texcoord.y = mesh->mTextureCoords[0][i].y;
        }
        modelData.vertices.push_back(vertex);
    }

    for (unsigned int i = 0; i < mesh->mNumFaces; ++i){
        const aiFace& face = mesh->mFaces[i];
        // 三角形想定
        modelData.indices.push_back(face.mIndices[0]);
        modelData.indices.push_back(face.mIndices[1]);
        modelData.indices.push_back(face.mIndices[2]);
    }
}

//----------------------------------------------------------------------------
// マテリアル読み込み
//----------------------------------------------------------------------------
void ModelManager::LoadMaterial(const aiScene* scene, const aiMesh* mesh, ModelData& modelData){
    if (!scene->HasMaterials()){
        modelData.material.textureFilePath = "white1x1.png";
        return;
    }
    const aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
    if (!material){
        modelData.material.textureFilePath = "white1x1.png";
        return;
    }

    aiString texPath;
    if (material->GetTexture(aiTextureType_DIFFUSE, 0, &texPath) == AI_SUCCESS){
        modelData.material.textureFilePath = texPath.C_Str();
    } else{
        modelData.material.textureFilePath = "white1x1.png";
    }

    LoadUVTransform(material, modelData.material);
}

//----------------------------------------------------------------------------
// UV変換情報
//----------------------------------------------------------------------------
void ModelManager::LoadUVTransform(const aiMaterial* material, MaterialData& outMaterial){
    aiUVTransform transformData;
    if (material->Get(AI_MATKEY_UVTRANSFORM(aiTextureType_DIFFUSE, 0), transformData) == AI_SUCCESS){
        outMaterial.uv_offset = {transformData.mTranslation.x, transformData.mTranslation.y, 0.0f};
        outMaterial.uv_scale = {transformData.mScaling.x, transformData.mScaling.y, 1.0f};
    } else{
        outMaterial.uv_offset = {0, 0, 0};
        outMaterial.uv_scale = {1, 1, 1};
    }
}

//----------------------------------------------------------------------------
// アニメーション評価サンプル
//----------------------------------------------------------------------------
Vector3 ModelManager::Evaluate(const AnimationCurve<Vector3>& curve, float time){
    const auto& keyframes = curve.keyframes;
    if (keyframes.empty()){
        return {0,0,0};
    }
    if (time <= keyframes.front().time){
        return keyframes.front().value;
    }
    if (time >= keyframes.back().time){
        return keyframes.back().value;
    }
    for (int i = 0; i < ( int ) keyframes.size() - 1; ++i){
        float t0 = keyframes[i].time;
        float t1 = keyframes[i + 1].time;
        if (time >= t0 && time <= t1){
            float localT = (time - t0) / (t1 - t0);
            return Vector3::Lerp(keyframes[i].value, keyframes[i + 1].value, localT);
        }
    }
    return keyframes.back().value;
}

Quaternion ModelManager::Evaluate(const AnimationCurve<Quaternion>& curve, float time){
    const auto& keyframes = curve.keyframes;
    if (keyframes.empty()){
        return {0,0,0,1};
    }
    if (time <= keyframes.front().time){
        return keyframes.front().value;
    }
    if (time >= keyframes.back().time){
        return keyframes.back().value;
    }
    for (int i = 0; i < ( int ) keyframes.size() - 1; ++i){
        float t0 = keyframes[i].time;
        float t1 = keyframes[i + 1].time;
        if (time >= t0 && time <= t1){
            float localT = (time - t0) / (t1 - t0);
            return Quaternion::Slerp(keyframes[i].value, keyframes[i + 1].value, localT);
        }
    }
    return keyframes.back().value;
}

#include "../objects/ModelManager.h"



ModelManager* ModelManager::instance_ = nullptr;
const std::string ModelManager::directoryPath_ = "Resources/models";

ModelManager* ModelManager::GetInstance(){
    if (!instance_){
        instance_ = new ModelManager();
    }
    return instance_;
}

void ModelManager::StartUpLoad(){
    LoadModel("suzanne.obj");
    //LoadModel("axis.obj");
   // LoadModel("bunny.obj");
    LoadModel("debugCube.obj");
    LoadModel("fence.obj");
    LoadModel("ground.obj");
    LoadModel("multiMaterial.obj");
    LoadModel("multiMesh.obj");
    LoadModel("plane.obj");
    LoadModel("teapot.obj");
    LoadModel("terrain.obj");
}


void ModelManager::Initialize(){
    GetInstance();
}

std::shared_ptr<ModelData> ModelManager::GetModelData(const std::string& fileName){
    auto it = instance_->modelDatas_.find(fileName);
    if (it != instance_->modelDatas_.end()){
        return it->second;
    }
    return nullptr; // ロードされていない場合はnullptrを返す
}

std::shared_ptr<ModelData> ModelManager::LoadModel(const std::string& fileName){
    if (instance_->modelDatas_.find(fileName) != instance_->modelDatas_.end()){
        return instance_->modelDatas_[fileName];
    }

    // モデルデータをロード
    std::shared_ptr<ModelData> newModel = std::make_shared<ModelData>(instance_->LoadModelFile(instance_->directoryPath_, fileName));
    instance_->modelDatas_[fileName] = newModel;

    Microsoft::WRL::ComPtr<ID3D12Device> device = GraphicsGroup::GetInstance()->GetDevice();

    // 頂点バッファとインデックスバッファの作成
    size_t vertexBufferSize = sizeof(VertexData) * newModel->vertices.size();
    instance_->vertexBuffers_[fileName] = CreateBufferResource(device, vertexBufferSize);

    size_t indexBufferSize = sizeof(uint32_t) * newModel->indices.size();
    instance_->indexBuffers_[fileName] = CreateBufferResource(device, indexBufferSize);

    // 頂点バッファへのデータマップ
    VertexData* vertexData = nullptr;
    instance_->vertexBuffers_[fileName]->Map(0, nullptr, reinterpret_cast< void** >(&vertexData));
    std::memcpy(vertexData, newModel->vertices.data(), vertexBufferSize);
    instance_->vertexBuffers_[fileName]->Unmap(0, nullptr);

    // インデックスバッファへのデータマップ
    uint32_t* indexData = nullptr;
    instance_->indexBuffers_[fileName]->Map(0, nullptr, reinterpret_cast< void** >(&indexData));
    std::memcpy(indexData, newModel->indices.data(), indexBufferSize);
    instance_->indexBuffers_[fileName]->Unmap(0, nullptr);

    return newModel;
}

Microsoft::WRL::ComPtr<ID3D12Resource> ModelManager::GetVertexResource(const std::string& fileName){
    auto it = instance_->vertexBuffers_.find(fileName);
    if (it != instance_->vertexBuffers_.end()){
        return it->second;
    }
    return nullptr; // 存在しない場合はnullptrを返す
}

Microsoft::WRL::ComPtr<ID3D12Resource> ModelManager::GetIndexResource(const std::string& fileName){
    auto it = instance_->indexBuffers_.find(fileName);
    if (it != instance_->indexBuffers_.end()){
        return it->second;
    }
    return nullptr; // 存在しない場合はnullptrを返す
}

void ModelManager::Finalize(){
    if (instance_){
        delete instance_;
        instance_ = nullptr;
    }
}

std::vector<std::string> ModelManager::GetLoadedModelNames() const{
    std::vector<std::string> modelNames;
    for (const auto& pair : modelDatas_){
        modelNames.push_back(pair.first);
    }
    return modelNames;
}

//====================================================================
// メイン：ファイルを読み込み、ModelData を返す
//====================================================================
ModelData ModelManager::LoadModelFile(const std::string& directoryPath, const std::string& fileNameWithExt){
    Assimp::Importer importer;

    // パス作成
    std::string filePath = directoryPath + "/"
        + fileNameWithExt.substr(0, fileNameWithExt.find_last_of('.')) + "/"
        + fileNameWithExt;

    const aiScene* scene = importer.ReadFile(
        filePath.c_str(),
        aiProcess_Triangulate
        | aiProcess_FlipUVs
        | aiProcess_CalcTangentSpace
    );
    assert(scene && "Failed to load file via Assimp");
    assert(scene->HasMeshes() && "No meshes found in the file");

    ModelData modelData;

    // メッシュ情報を読み取る
    for (unsigned int meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex){
        const aiMesh* mesh = scene->mMeshes[meshIndex];
        // 頂点/インデックス読み込み
        LoadMesh(mesh, modelData);
        // マテリアル読み込み
        LoadMaterial(scene, mesh, modelData);
    }

    // *********************
    //   アニメーション読み込み
    // *********************
    if (scene->HasAnimations()){
        // とりあえず 0 番目のアニメーションだけ読み込む例
        aiAnimation* aiAnim = scene->mAnimations[0];

        Animation animation;
        // Assimpのアニメーションの尺(単位: tick)
        // glTFの場合: `mDuration` はアニメーションの tick 数, `mTicksPerSecond` が 1秒あたりのtick数
        float ticksPerSecond = ( float ) (aiAnim->mTicksPerSecond != 0 ? aiAnim->mTicksPerSecond : 25.0);
        animation.duration = ( float ) (aiAnim->mDuration / ticksPerSecond);

        // 各チャンネルの読み取り (1チャンネル ≈ 1ノード(ボーン)が持つトランスフォームアニメーション)
        for (unsigned int channelIdx = 0; channelIdx < aiAnim->mNumChannels; ++channelIdx){
            aiNodeAnim* nodeAnim = aiAnim->mChannels[channelIdx];
            NodeAnimation nodeAnimation;

            // (A) Translationキー
            for (unsigned int i = 0; i < nodeAnim->mNumPositionKeys; ++i){
                float time = ( float ) (nodeAnim->mPositionKeys[i].mTime / ticksPerSecond);

                aiVector3D aiPos = nodeAnim->mPositionKeys[i].mValue;
                Vector3 position = {aiPos.x, aiPos.y, aiPos.z};

                nodeAnimation.translate.keyframes.push_back({time, position});
            }

            // (B) Rotationキー (クォータニオン)
            for (unsigned int i = 0; i < nodeAnim->mNumRotationKeys; ++i){
                float time = ( float ) (nodeAnim->mRotationKeys[i].mTime / ticksPerSecond);

                aiQuaternion aiRot = nodeAnim->mRotationKeys[i].mValue;
                Quaternion rotation = {aiRot.x, aiRot.y, aiRot.z, aiRot.w};

                nodeAnimation.rotate.keyframes.push_back({time, rotation});
            }

            // (C) Scalingキー
            for (unsigned int i = 0; i < nodeAnim->mNumScalingKeys; ++i){
                float time = ( float ) (nodeAnim->mScalingKeys[i].mTime / ticksPerSecond);

                aiVector3D aiScale = nodeAnim->mScalingKeys[i].mValue;
                Vector3 scale = {aiScale.x, aiScale.y, aiScale.z};

                nodeAnimation.scale.keyframes.push_back({time, scale});
            }

            // ノード名(ボーン名など)をキーにして登録
            std::string nodeName(nodeAnim->mNodeName.C_Str());
            animation.nodeAnimations[nodeName] = nodeAnimation;
        }

        // 必要に応じて ModelData にアニメーションを格納 (複数あるなら配列に push_back など)
        // ここでは単一アニメーション想定の例
        modelData.animation = animation;
    }

    return modelData;
}

//====================================================================
// メッシュを読み込む (頂点とインデックス)
//====================================================================
void ModelManager::LoadMesh(const aiMesh* mesh, ModelData& modelData){
    // 頂点 (位置, 法線, UV)
    for (unsigned int i = 0; i < mesh->mNumVertices; i++){
        VertexData vertex {};
        vertex.position.x = mesh->mVertices[i].x;
        vertex.position.y = mesh->mVertices[i].y;
        vertex.position.z = mesh->mVertices[i].z;
        vertex.position.w = 1.0f;

        if (mesh->HasNormals()){
            vertex.normal.x = mesh->mNormals[i].x;
            vertex.normal.y = mesh->mNormals[i].y;
            vertex.normal.z = mesh->mNormals[i].z;
        }
        if (mesh->HasTextureCoords(0)){
            vertex.texcoord.x = mesh->mTextureCoords[0][i].x;
            vertex.texcoord.y = mesh->mTextureCoords[0][i].y;
        } else{
            vertex.texcoord = {0.0f, 0.0f};
        }

        modelData.vertices.push_back(vertex);
    }

    // インデックス
    for (unsigned int i = 0; i < mesh->mNumFaces; i++){
        const aiFace& face = mesh->mFaces[i];
        assert(face.mNumIndices == 3); // 三角形のみ
        modelData.indices.push_back(face.mIndices[0]);
        modelData.indices.push_back(face.mIndices[1]);
        modelData.indices.push_back(face.mIndices[2]);
    }
}

//====================================================================
// マテリアルを読み込む (テクスチャパスと UV変換など)
//====================================================================
void ModelManager::LoadMaterial(const aiScene* scene, const aiMesh* mesh, ModelData& modelData){
    // メッシュに関連づいたマテリアルを取得
    if (scene->HasMaterials()){
        const aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        if (!material){
            // 何も無いときはデフォルトの白テクスチャなどにする
            modelData.material.textureFilePath = "white1x1.png";
            return;
        }

        // (A) テクスチャパスを取得 (Diffuse または glTFのBaseColor)
        aiString texPath;
        if (material->GetTexture(aiTextureType_DIFFUSE, 0, &texPath) == AI_SUCCESS){
            modelData.material.textureFilePath = texPath.C_Str();
        } else{
            // テクスチャが無い場合はフォールバック
            modelData.material.textureFilePath = "white1x1.png";
        }

        // (B) UV変換(KHR_texture_transform)があれば読み込む
        LoadUVTransform(material, modelData.material);
    }
}

//====================================================================
// KHR_texture_transform などのUV変換を読む
//====================================================================
void ModelManager::LoadUVTransform(const aiMaterial* material, MaterialData& outMaterial){
    aiUVTransform transformData;
    if (material->Get(AI_MATKEY_UVTRANSFORM(aiTextureType_DIFFUSE, 0), transformData) == AI_SUCCESS){
        // Translation (オフセット)
        outMaterial.uv_offset.x = transformData.mTranslation.x;
        outMaterial.uv_offset.y = transformData.mTranslation.y;
        // スケール
        outMaterial.uv_scale.x = transformData.mScaling.x;
        outMaterial.uv_scale.y = transformData.mScaling.y;
        // 回転 (Assimpの uvTransform は 2D回転, Z軸まわりの radians)
        //outMaterial.uv_rotate.z = transformData.Rotation;
    } else{
        // 変換情報が無い場合はデフォルト値
        outMaterial.uv_offset = {0, 0, 0};
        outMaterial.uv_scale = {1, 1, 1};
       // outMaterial.uv_rotate = {0, 0, 0};
    }
}

Vector3 ModelManager::Evaluate(const AnimationCurve<Vector3>& curve, float time){
    const auto& keyframes = curve.keyframes;
    if (keyframes.empty()){
        // キーフレームが無ければデフォルト
        return {0,0,0};
    }
    if (time <= keyframes.front().time){
        // 開始より前なら先頭キーの値
        return keyframes.front().value;
    }
    if (time >= keyframes.back().time){
        // 終了より後なら末尾キーの値 (ループしない場合)
        return keyframes.back().value;
    }
    // 途中: time を挟む2つのキーを探して補間
    for (int i = 0; i < ( int ) keyframes.size() - 1; ++i){
        float t0 = keyframes[i].time;
        float t1 = keyframes[i + 1].time;
        if (time >= t0 && time <= t1){
            float localT = (time - t0) / (t1 - t0);
            return Vector3::Lerp(keyframes[i].value, keyframes[i + 1].value, localT);
        }
    }
    return keyframes.back().value; // fallback
}

Quaternion ModelManager::Evaluate(const AnimationCurve<Quaternion>& curve, float time){
    const auto& keyframes = curve.keyframes;
    if (keyframes.empty()){
        return {0,0,0,1}; // default identity
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
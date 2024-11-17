#include "objects/ModelManager.h"

ModelManager* ModelManager::instance_ = nullptr;
const std::string ModelManager::directoryPath_ = "Resources/models";

ModelManager* ModelManager::GetInstance(){
    if (!instance_){
        instance_ = new ModelManager();
    }
    return instance_;
}

void ModelManager::StartUpLoad(){
    LoadModel("suzanne");
    //LoadModel("axis");
    //LoadModel("bunny");
    LoadModel("debugCube");
    LoadModel("fence");
    LoadModel("ground");
    LoadModel("multiMaterial");
    LoadModel("multiMesh");
    LoadModel("plane");
    LoadModel("suzanne");
    LoadModel("teapot");
    LoadModel("terrain");
    LoadModel("scaffold");
    LoadModel("coaster");
    LoadModel("station");
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
    std::shared_ptr<ModelData> newModel = std::make_shared<ModelData>(LoadObjFile(instance_->directoryPath_, fileName));
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
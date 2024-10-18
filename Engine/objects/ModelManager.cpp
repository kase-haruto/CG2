#include "objects/ModelManager.h"

ModelManager* ModelManager::instance_ = nullptr;
const std::string ModelManager::directoryPath_ = "Resources/models";

ModelManager* ModelManager::GetInstance(){
    // インスタンスがなければ作成
    if (!instance_){ instance_ = new ModelManager();}

    return instance_;
}

void ModelManager::Initialize(){
    //インスタンスがなければ作る
    GetInstance();
}

std::shared_ptr<ModelData> ModelManager::GetModelData(const std::string& fileName){
    // 既にロードされているか確認
    assert(instance_->modelDatas_.find(fileName) != instance_->modelDatas_.end());
    return instance_->modelDatas_[fileName];
}

std::shared_ptr<ModelData> ModelManager::LoadModel(const std::string& fileName){
    // すでにロードされている場合は再利用
    if (instance_->modelDatas_.find(fileName) != instance_->modelDatas_.end()){
        return instance_->modelDatas_[fileName];
    }

    // 新しくモデルを読み込む
    std::shared_ptr<ModelData> newModel = std::make_shared<ModelData>(LoadObjFile(instance_->directoryPath_, fileName));
    instance_->modelDatas_[fileName] = newModel;

    return newModel;
}

void ModelManager::Finalize(){
    // インスタンスを削除
    if (instance_){
        delete instance_;
        instance_ = nullptr;
    }
}

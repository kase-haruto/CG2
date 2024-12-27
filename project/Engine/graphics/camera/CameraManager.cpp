#include "CameraManager.h"

CameraManager* CameraManager::instance_ = nullptr;

CameraManager* CameraManager::GetInstance(){
	if (!instance_){
		instance_ = new CameraManager();
	}
	return instance_;
}

CameraManager::CameraManager(){
	camera3d_ = std::make_unique<Camera3d>();
	followCamera_ = std::make_unique<FollowCamera>();
	debugCamera_ = std::make_unique<DebugCamera>();

	cameras_[Type_Default] = camera3d_.get();
	cameras_[Type_Follow] = followCamera_.get();
	cameras_[Type_Debug] = debugCamera_.get();
}

void CameraManager::SetType(const CameraType type){
	type_ = type;
	for (auto& camera : cameras_){
		camera.second->SetActive(camera.first == type_);
	}
}

void CameraManager::Initialize(){
	GetInstance();//インスタンスがない場合作成
}

void CameraManager::Update(){

	//アクティブなカメラのみ更新
	for (auto& camera:instance_->cameras_){
		camera.second->Update();
	}

}

void CameraManager::Finalize(){
	if (instance_){ delete instance_; }
}
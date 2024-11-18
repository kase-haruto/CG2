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
}

void CameraManager::Initialize(){
	GetInstance();//インスタンスがない場合作成	
}

void CameraManager::Update(){
	instance_->camera3d_->Update();
}

void CameraManager::Finalize(){
	instance_->camera3d_.reset();
	if (instance_){	delete instance_;}
}

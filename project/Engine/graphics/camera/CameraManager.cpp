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
}

void CameraManager::Initialize(){
	GetInstance();//インスタンスがない場合作成
	instance_->type_ = Type_Normal;
}

void CameraManager::Update(){
	instance_->camera3d_->Update();
	instance_->followCamera_->Update();

	if (instance_->type_ == Type_Follow){
		instance_->camera3d_->SetCamera(instance_->followCamera_->GetTranslate(), instance_->followCamera_->GetRotate());
	}
}

void CameraManager::Finalize(){
	instance_->camera3d_.reset();
	if (instance_){ delete instance_; }
}
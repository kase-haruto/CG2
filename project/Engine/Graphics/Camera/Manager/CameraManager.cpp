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

void CameraManager::SetViewportSize(ViewportType type, const Vector2& size) {
	switch (type) {
		case ViewportType::VIEWPORT_MAIN:
			mainViewportSize_ = size;
			break;
		case ViewportType::VIEWPORT_DEBUG:
			debugViewportSize_ = size;
			break;
		default:
			break;
	}
}

Vector2 CameraManager::GetViewportSize(ViewportType type) const {
	switch (type) {
		case ViewportType::VIEWPORT_MAIN:
			return mainViewportSize_;
		case ViewportType::VIEWPORT_DEBUG:
			return debugViewportSize_;
		default:
			return Vector2{ 0, 0 };
	}
}

void CameraManager::Initialize(){
	GetInstance();//インスタンスがない場合作成
}

void CameraManager::Update(){

	for (auto& camera:instance_->cameras_){
		camera.second->Update();
	}

}

void CameraManager::Finalize(){
	if (instance_){ delete instance_; }
}

void CameraManager::TransfarToGPU(){
	for (auto& camera : instance_->cameras_){
		camera.second->TransfarToGPU();
	}
}

void CameraManager::SetAspectRatio(float width, float height){
	if (height <= 0) return;
	float aspect = width / height;

	for (auto& camera : cameras_){
		camera.second->SetAspectRatio(aspect);
	}
}


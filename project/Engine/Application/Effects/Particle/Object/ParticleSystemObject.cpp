#include "ParticleSystemObject.h"

#include <Engine/Foundation/Clock/ClockManager.h>
#include <Engine/System/Command/EditorCommand/GuiCommand/ImGuiHelper/GuiCmd.h>

ParticleSystemObject::ParticleSystemObject(const std::string& name){
	SceneObject::SetName(name, ObjectType::ParticleSystem);
}

void ParticleSystemObject::Initialize(){
	// 必要に応じて初期化処理
}

void ParticleSystemObject::Update(){
	// トランスフォームの更新
	worldTransform_.Update();
	position_ = worldTransform_.GetWorldPosition();

	// 自身の emitter を更新
	FxEmitter::Update();

	// 子オブジェクトの再帰更新
	for (const auto& child : children_){
		if (auto* ps = dynamic_cast< ParticleSystemObject* >(child)){
			ps->Update();
		}
	}
}

void ParticleSystemObject::ShowGui(){
	ConfigurableObject::ShowGUi();
	FxEmitter::ShowGui();

	// 子の GUI も展開
	/*for (const auto& child : children_){
		if (auto* ps = dynamic_cast< ParticleSystemObject* >(child)){
			ps->ShowGui();
		}
	}*/
}

void ParticleSystemObject::SetDrawEnable(bool isDrawEnable){
	FxEmitter::SetDrawEnable(isDrawEnable);

	// 子にも適用
	for (const auto& child : children_){
		if (auto* ps = dynamic_cast< ParticleSystemObject* >(child)){
			ps->SetDrawEnable(isDrawEnable);
		}
	}
}

void ParticleSystemObject::ApplyConfig(){
	// FxEmitter 設定反映
	FxEmitter::ApplyConfigFrom(config_); // config_ は ParticleSystemObjectConfig のはず

	// SceneObject 情報
	name_ = config_.name;
	id_ = config_.guid;
	parentId_ = config_.parentGuid;

	worldTransform_.ApplyConfig(config_.transform);
}

void ParticleSystemObject::ExtractConfig(){
	FxEmitter::ExtractConfigTo(config_); // config_ は ParticleSystemObjectConfig

	config_.name = name_;
	config_.guid = id_;
	config_.parentGuid = parentId_;
	worldTransform_.ExtractConfig();
}

void ParticleSystemObject::PlayRecursive(){
	Play();
	for (const auto& child : children_){
		if (auto* ps = dynamic_cast< ParticleSystemObject* >(child)){
			ps->PlayRecursive();
		}
	}
}

void ParticleSystemObject::StopRecursive(){
	Stop();
	for (const auto& child : children_){
		if (auto* ps = dynamic_cast< ParticleSystemObject* >(child)){
			ps->StopRecursive();
		}
	}
}

void ParticleSystemObject::ResetRecursive(){
	Reset();
	for (const auto& child : children_){
		if (auto* ps = dynamic_cast< ParticleSystemObject* >(child)){
			ps->ResetRecursive();
		}
	}
}

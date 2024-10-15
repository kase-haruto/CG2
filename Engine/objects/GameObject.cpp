#include "Game/objects/GameObject.h"

void GameObject::Initialize(Model* model){
	model_ = model;
	model_->SetViewProjection();
}

void GameObject::Update(){

}

void GameObject::Draw(){

}

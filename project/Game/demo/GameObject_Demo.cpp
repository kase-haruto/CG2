#include "GameObject_Demo.h"

#include "Engine/collision/CollisionManager.h"


GameObject_Demo::GameObject_Demo(const std::string& modelName)
:BaseGameObject(modelName){

	particle_ = std::make_unique<DemoParticle>();

	BoxCollider::Initialize(model_->transform.scale);

	CollisionManager::GetInstance()->AddCollider(this);

}

GameObject_Demo::~GameObject_Demo(){

	CollisionManager::GetInstance()->RemoveCollider(this);

}

void GameObject_Demo::Initialize(){

	BaseGameObject::SetName("debugObject");

	particle_->Initialize("plane","particle.png");

}

void GameObject_Demo::Update(){

	particle_->SetEmitPos(model_->transform.translate);

	particle_->Update();

	shape_.center = model_->transform.translate;
	shape_.rotate = model_->transform.rotate;
	shape_.size = model_->transform.scale+0.1f;

	BaseGameObject::Update();

}

void GameObject_Demo::Draw(){

	BoxCollider::Draw();
	BaseGameObject::Draw();

}


//===================================================================*/
//                    collision
//===================================================================*/

void GameObject_Demo::OnCollision([[maybe_unused]]Collider* other){}


//===================================================================*/
//                    imgui/ui
//===================================================================*/
void GameObject_Demo::ShowDebugUI(){

	BaseGameObject::ShowDebugUI();

}



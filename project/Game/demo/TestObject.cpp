#include "TestObject.h"
#include "Engine/collision/CollisionManager.h"

TestObject::TestObject(const std::string& modelName)
:BaseGameObject(modelName){

	SphereCollider::Initialize(model_->transform.scale.x * 0.5f + 0.1f);

	CollisionManager::GetInstance()->AddCollider(this);
}

TestObject::~TestObject(){

	CollisionManager::GetInstance()->RemoveCollider(this);

}

void TestObject::Initialize(){


	BaseGameObject::SetName("collisionTestObject");
}

void TestObject::Update(){

	shape_.center = model_->transform.translate;

	BaseGameObject::Update();

}

void TestObject::Draw(){

	SphereCollider::Draw();
	BaseGameObject::Draw();

}


void TestObject::OnCollision([[maybe_unused]]Collider* other){}

//===================================================================*/
//                    imgui/ui
//===================================================================*/
void TestObject::ShowDebugUI(){

	BaseGameObject::ShowDebugUI();
}


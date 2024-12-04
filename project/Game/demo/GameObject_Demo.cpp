#include "GameObject_Demo.h"

GameObject_Demo::GameObject_Demo(const std::string& modelName)
:BaseGameObject(modelName){

	particle_ = std::make_unique<DemoParticle>();

}

void GameObject_Demo::Initialize(){

	GameObject_Demo::SetName("debugObject");

	particle_->Initialize("plane");

}

void GameObject_Demo::Update(){

	particle_->Update();

	BaseGameObject::Update();

}

void GameObject_Demo::Draw(){

	BaseGameObject::Draw();

}


//===================================================================*/
//                    imgui/ui
//===================================================================*/
void GameObject_Demo::ShowDebugUI(){

	BaseGameObject::ShowDebugUI();

}



#include "HorizonMowingDown.h"

HorizonMowingDown::HorizonMowingDown(const std::string& attackName)
:IPlayerAttack(attackName){}

//////////////////////////////////////////////////////////////////////////
//						main functions
//////////////////////////////////////////////////////////////////////////
void HorizonMowingDown::Initialize(){}

void HorizonMowingDown::Execution(){


	IPlayerAttack::Execution();
}

void HorizonMowingDown::Update(){
	if (!isAttacking_){ return; }
	IPlayerAttack::Update();
}

void HorizonMowingDown::Draw(){
	IPlayerAttack::Draw();
}

//////////////////////////////////////////////////////////////////////////
//						imgui/ui
//////////////////////////////////////////////////////////////////////////
void HorizonMowingDown::ShowGui(){
	IPlayerAttack::ShowGui();


}

//////////////////////////////////////////////////////////////////////////
//						collision
//////////////////////////////////////////////////////////////////////////
void HorizonMowingDown::OnCollisionEnter([[maybe_unused]]Collider* other){}

void HorizonMowingDown::OnCollisionStay([[maybe_unused]] Collider* other){}

void HorizonMowingDown::OnCollisionExit([[maybe_unused]] Collider* other){}

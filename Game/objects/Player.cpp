#include "objects/Player.h"

void Player::Initialize(Model* model){
	Character::Initialize(model);

}

void Player::Update(){

	//行列の更新
	Character::Update();
}

void Player::Draw(){
	Character::Draw();
}

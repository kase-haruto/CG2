#include "objects/Player.h"

void Player::Initialize(Model* model){
	Character::Initialize(model);

}

void Player::Update(){

	//�s��̍X�V
	Character::Update();
}

void Player::Draw(){
	Character::Initialize();
}

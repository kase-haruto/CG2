#include "MoveCommand.h"
#include "Player.h"

void MoveRightCommand::Execute(Player& player){
	player.MoveRight();
}

void MoveLeftCommand::Execute(Player& player){
	player.MoveLeft();
}

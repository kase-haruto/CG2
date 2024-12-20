#pragma once

#include"ICommand.h"


//===================================================================*/
//                    move to right
//===================================================================*/
class MoveRightCommand
	:public ICommand{
public:
	MoveRightCommand() = default;
	~MoveRightCommand()override = default;

	void Execute(Player& player)override;
};

//===================================================================*/
//                    move to left
//===================================================================*/
class MoveLeftCommand
	:public ICommand{
public:
	MoveLeftCommand() = default;
	~MoveLeftCommand()override = default;

	void Execute(Player& player)override;
};
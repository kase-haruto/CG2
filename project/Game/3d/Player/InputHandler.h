#pragma once

#include "MoveCommand.h"

class InputHandler{
public:
	InputHandler() = default;
	~InputHandler() = default;


	ICommand* HandleInput();

	void AssignMoveLeftCommand2PressKeyA();
	void AssignMoveRightCommand2PressKeyD();
private:
	ICommand* pressKeyD_;
	ICommand* pressKeyA_;
};


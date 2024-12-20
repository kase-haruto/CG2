#include "InputHandler.h"

#include "Engine/core/Input.h"

ICommand* InputHandler::HandleInput(){
	if (Input::PushKey(DIK_D)){
		return pressKeyD_;
	} else if (Input::PushKey(DIK_A)){
		return pressKeyA_;
	}

	return nullptr;
}

void InputHandler::AssignMoveLeftCommand2PressKeyA(){
	ICommand* command = new MoveLeftCommand();
	pressKeyA_ = command;
}

void InputHandler::AssignMoveRightCommand2PressKeyD(){
	ICommand* command = new MoveRightCommand();
	pressKeyD_ = command;
}

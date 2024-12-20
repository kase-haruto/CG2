#pragma once

//forward declaration
class Player;

class ICommand{
public:
	ICommand() = default;
	virtual ~ICommand() = default;

	virtual void Execute(Player& player) = 0;

private:

};

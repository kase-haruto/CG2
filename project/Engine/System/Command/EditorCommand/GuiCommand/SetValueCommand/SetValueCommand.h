#pragma once

#include <Engine/System/Command/Interface/ICommand.h>
#include <functional>

template<typename T>
class SetValueCommand
	: public ICommand{
public:
	SetValueCommand(const T& before, const T& after, std::function<void(const T&)> setter)
		: before_(before), after_(after), setter_(std::move(setter)){}

	void Execute() override{ setter_(after_); }
	void Undo() override{ setter_(before_); }
	void Redo() override{ Execute(); }

private:
	T before_;
	T after_;
	std::function<void(const T&)> setter_;
};
#include "CommandManager.h"

CommandManager* CommandManager::GetInstance() {
	static CommandManager instance;
	return &instance;
}

void CommandManager::Execute(std::unique_ptr<ICommand> cmd) {
	cmd->Execute();
	undoStack_.push(std::move(cmd));
	while (!redoStack_.empty()) redoStack_.pop();
}

void CommandManager::Undo() {
	if (undoStack_.empty()) return;
	auto cmd = std::move(undoStack_.top()); undoStack_.pop();
	cmd->Undo();
	redoStack_.push(std::move(cmd));
}

void CommandManager::Redo() {
	if (redoStack_.empty()) return;
	auto cmd = std::move(redoStack_.top()); redoStack_.pop();
	cmd->Redo();                               // Execute ではなく Redo を明示
	undoStack_.push(std::move(cmd));
}
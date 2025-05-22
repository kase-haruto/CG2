#pragma once
#include <Engine/System/Command/Interface/ICommand.h>
#include <Engine/System/Command/EditorCommand/TransformCommand/TransformSnapshot.h>

class TranslateCommand :
	public ICommand {
public:
	TranslateCommand(SceneObject* obj, const Vector3& delta)
		: object_(obj), delta_(delta) {}

	void Execute() override;
	void Undo() override;

private:
	SceneObject* object_;
	Vector3            delta_;
	TransformSnapshot  before_;
};
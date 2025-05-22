#pragma once

#include <Engine/System/Command/Interface/ICommand.h>
#include <Engine/System/Command/EditorCommand/TransformCommand/TransformSnapshot.h>

class RotateCommand :
	public ICommand {
public:
	RotateCommand(SceneObject* obj, const Vector3& axis, float rad)
		: object_(obj), qDelta_(Quaternion::MakeRotateAxisQuaternion(axis, rad)) {}

	RotateCommand(SceneObject* obj, const Quaternion& q):
		object_(obj), qDelta_(q) {}

	void Execute() override;
	void Undo() override;

private:
	SceneObject* object_;
	Quaternion qDelta_;
	TransformSnapshot before_;
};
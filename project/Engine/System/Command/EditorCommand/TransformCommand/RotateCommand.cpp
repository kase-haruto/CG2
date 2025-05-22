#include "RotateCommand.h"

void RotateCommand::Execute() {
	before_ = TransformSnapshot::Capture(object_);
	auto& rot = object_->GetWorldTransform().rotation;
	rot = qDelta_ * rot;
}

void RotateCommand::Undo() { before_.Apply(object_); }

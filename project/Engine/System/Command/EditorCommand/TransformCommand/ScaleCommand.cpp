#include "ScaleCommand.h"

void ScaleCommand::Execute() {
    before_ = TransformSnapshot::Capture(object_);
    auto& s = object_->GetWorldTransform().scale;
    s = s * factor_;
}

void ScaleCommand::Undo() { before_.Apply(object_); }

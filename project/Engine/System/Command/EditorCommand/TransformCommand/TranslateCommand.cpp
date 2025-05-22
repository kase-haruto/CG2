#include "TranslateCommand.h"

void TranslateCommand::Execute() {
    before_ = TransformSnapshot::Capture(object_);
    object_->GetWorldTransform().translation += delta_;
}

void TranslateCommand::Undo() { before_.Apply(object_); }

#pragma once

#include <Engine/System/Command/Interface/ICommand.h>
#include <Engine/System/Command/EditorCommand/TransformCommand/TransformSnapshot.h>

class GenericTransformCommand :
    public ICommand {
public:
    GenericTransformCommand(SceneObject* obj,
                            const TransformSnapshot& before,
                            const TransformSnapshot& after)
        : obj_(obj), before_(before), after_(after) {}

    void Execute() override {}

    void Undo() override { before_.Apply(obj_); }
    void Redo() override { after_.Apply(obj_); }

private:
    SceneObject* obj_;
    TransformSnapshot   before_, after_;
};

#pragma once
#include <Engine/System/Command/EditorCommand/TransformCommand/TransformSnapshot.h>
#include <Engine/System/Command/Interface/ICommand.h>

class ScaleCommand :
    public ICommand {
public:
    ScaleCommand(SceneObject* obj, const Vector3& factor)
        : object_(obj), factor_(factor) {}

    void Execute() override;
    void Undo() override;

private:
    SceneObject* object_;
    Vector3            factor_;
    TransformSnapshot  before_;
};


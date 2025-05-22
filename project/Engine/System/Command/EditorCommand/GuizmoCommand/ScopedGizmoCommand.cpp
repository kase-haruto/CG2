#include "ScopedGizmoCommand.h"
#include <Engine/System/Command/EditorCommand/TransformCommand/TranslateCommand.h>
#include <Engine/System/Command/EditorCommand/TransformCommand/RotateCommand.h>
#include <Engine/System/Command/EditorCommand/TransformCommand/ScaleCommand.h>


ScopedGizmoCommand::ScopedGizmoCommand(SceneObject* obj, ImGuizmo::OPERATION op)
	: obj_(obj), before_(TransformSnapshot::Capture(obj)),op_(op) {}

ScopedGizmoCommand::~ScopedGizmoCommand() {
    TransformSnapshot after = TransformSnapshot::Capture(obj_);
    if (memcmp(&before_, &after, sizeof(before_)) == 0) return;   // 変化なし

    Vector3   dT = after.translate - before_.translate;
    Vector3   dS = after.scale / before_.scale;
    Quaternion dQ = after.rotate * Quaternion::Inverse(before_.rotate);

    auto& mgr = *CommandManager::GetInstance();
    switch (op_) {
        case ImGuizmo::TRANSLATE:
            if (dT.LengthSquared() > 1e-6f)
                mgr.Execute(std::make_unique<TranslateCommand>(obj_, dT));
            break;
        case ImGuizmo::ROTATE:
            if (dQ.NotIdentity())
                mgr.Execute(std::make_unique<RotateCommand>(obj_, dQ));
            break;
        case ImGuizmo::SCALE:
            if (dS != Vector3{ 1,1,1 })
                mgr.Execute(std::make_unique<ScaleCommand>(obj_, dS));
            break;
    }
}
#pragma once

#include <Engine/System/Command/Interface/ICommand.h>
#include <Engine/System/Command/Manager/CommandManager.h>
#include <Engine/System/Command/EditorCommand/TransformCommand/GenericTransformCommand.h>

#include <externals/imgui/imgui.h>
#include <externals/imgui/ImGuizmo.h>


class ScopedGizmoCommand {
public:
	ScopedGizmoCommand(SceneObject* obj, ImGuizmo::OPERATION op);

	~ScopedGizmoCommand();

	// コピー防止
	ScopedGizmoCommand(const ScopedGizmoCommand&) = delete;
	ScopedGizmoCommand& operator=(const ScopedGizmoCommand&) = delete;

private:
	SceneObject* obj_;
	ImGuizmo::OPERATION op_;
	TransformSnapshot   before_;
};
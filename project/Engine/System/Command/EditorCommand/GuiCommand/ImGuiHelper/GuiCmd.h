#pragma once

#include <Engine/System/Command/Interface/ICommand.h>
#include <Engine/System/Command/Manager/CommandManager.h>
#include <Engine/System/Command/EditorCommand/GuiCommand/SetValueCommand/SetValueCommand.h>
#include <Engine/Foundation/Math/Vector3.h>

#include <functional>
#include <cmath>
#include <string>
#include <memory>

#include <externals/imgui/imgui.h>

namespace GuiCmd{

	bool DragFloat3(const char* label,Vector3& value,float speed = 0.01f,float min = 0.0f,float max = 0.0f);

}
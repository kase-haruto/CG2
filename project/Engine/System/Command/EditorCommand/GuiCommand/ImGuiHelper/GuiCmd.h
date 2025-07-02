#pragma once
/* ========================================================================
/*		include space
/* ===================================================================== */

// engine
#include <Engine/System/Command/Interface/ICommand.h>
#include <Engine/System/Command/Manager/CommandManager.h>
#include <Engine/System/Command/EditorCommand/GuiCommand/SetValueCommand/SetValueCommand.h>

// c++
#include <functional>
#include <cmath>
#include <string>
#include <memory>

// external
#include <externals/imgui/imgui.h>

// math
struct Vector2;
struct Vector3;
struct Vector4;

namespace GuiCmd{

	//===================================================================*/
	//		dragInt
	//===================================================================*/
	bool DragInt(const char* label, int& value, float speed = 0.01f, float min = 0.0f, float max = 0.0f);

	//===================================================================*/
	//		dragFloat
	//===================================================================*/
	bool DragFloat(const char* label, float& value, float speed = 0.01f, float min = 0.0f, float max = 0.0f);
	bool DragFloat2(const char* label, Vector2& value, float speed = 0.01f, float min = 0.0f, float max = 0.0f);
	bool DragFloat3(const char* label,Vector3& value,float speed = 0.01f,float min = 0.0f,float max = 0.0f);
	bool DragFloat4(const char* label, Vector4& value, float speed = 0.01f, float min = 0.0f, float max = 0.0f);

	//===================================================================*/
	//		sliderFloat
	//===================================================================*/
	bool SliderFloat(const char* label, float& value, float min = 0.0f, float max = 1.0f);
	bool SliderFloat2(const char* label, Vector2& value, float min = 0.0f, float max = 1.0f);
	bool SliderFloat3(const char* label, Vector3& value, float min = 0.0f, float max = 1.0f);
	bool SliderFloat4(const char* label, Vector4& value, float min = 0.0f, float max = 1.0f);

	//===================================================================*/
	//		colorEdit
	//===================================================================*/
	bool ColorEdit4(const char* label, Vector4& value, ImGuiColorEditFlags flags = 0);

	//===================================================================*/
	//		checkbox
	//===================================================================*/
	bool CheckBox(const char* label, bool& value);
}
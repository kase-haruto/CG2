#pragma once
#include <stdint.h>
#include <string>

#include <lib/myMath/Vector2.h>

static const std::string windowTitle = "Engine";

//フルhdとhdの中間1280x720と1920x1080,1600x900
static const uint32_t kWindowWidth = 1888;
static const uint32_t kWindowHeight = 1062;

static const Vector2 kWindowSize = Vector2(
	static_cast<float>(kWindowWidth), static_cast< float >(kWindowHeight));

//imGUiの実行ウィンドウサイズ736x414:16:9
static const uint32_t kGuiWindowWidth = 1024;
static const uint32_t kGuiWindowHeight = 576;

static const Vector2 kExecuteWindowSize = Vector2(
	static_cast< float >(kGuiWindowWidth), static_cast< float >(kGuiWindowHeight));

static const Vector2 kGameViewSize = Vector2(
	static_cast< float >(kGuiWindowWidth) * 0.7f, static_cast< float >(kGuiWindowHeight) * 0.7f);
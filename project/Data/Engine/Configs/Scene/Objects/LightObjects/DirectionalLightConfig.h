#pragma once
/* ========================================================================
/*  include space
/* ===================================================================== */
#include <Engine/Foundation/Math/Vector4.h>
#include <Engine/Foundation/Math/Vector3.h>

#include <string>

struct DirectionalLightConfig final {
	//========================= variable =========================
	std::string sceneName;						//< シーン名
	Vector4 color = { 1.0f, 1.0f, 1.0f, 1.0f };	//< 光の色
	Vector3 direction = { 0.0f, -1.0f, 0.0f };	//< 光の方向
	float intensity = 0.25f;					//< 光の強度
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(DirectionalLightConfig, sceneName, color, direction, intensity)

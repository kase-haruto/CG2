#pragma once
/* ========================================================================
/*  include space
/* ===================================================================== */
#include <Engine/Foundation/Math/Vector4.h>
#include <Engine/Foundation/Math/Vector3.h>

#include <string>

struct PointLightConfig final {
	//========================= variable =========================
	std::string sceneName;						//< シーン名
	Vector4 color = { 1.0f, 1.0f, 1.0f, 1.0f };	//< 光の色
	Vector3 position = { 0.0f, 0.0f, 0.0f };	//< 位置
	float intensity = 0.25f;					//< 光の強度
	float radius = 20.0f;						//< ライトの届く最大距離
	float decay = 1.0f;							//< 減衰率
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(PointLightConfig,
								   sceneName,
								   color,
								   position,
								   intensity,
								   radius,
								   decay)

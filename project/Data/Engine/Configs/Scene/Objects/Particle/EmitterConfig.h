#pragma once

#include <Data/Engine/Configs/Scene/Objects/Particle/FxParmConfig.h>
#include <Engine/Foundation/Math/Vector3.h>

#include <string>
#include <nlohmann/json.hpp>

struct EmitterConfig{
	Vector3 position {}; // パーティクルの発生位置
	// パーティクル生成パラメータ
	FxVector3ParamConfig velocity;
	FxFloatParamConfig lifetime;

	// 基本プロパティ
	float emitRate = 0.1f;

	// リソースパス
	std::string modelPath = "plane.obj";
	std::string texturePath = "particle.png";

	// フラグ
	bool isDrawEnable = true;
	bool isComplement = true;
	bool isStatic = false;
};

// JSONシリアライズ対応
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(EmitterConfig,
								   position,
								   velocity,
								   lifetime,
								   emitRate,
								   modelPath,
								   texturePath,
								   isDrawEnable,
								   isComplement,
								   isStatic
)

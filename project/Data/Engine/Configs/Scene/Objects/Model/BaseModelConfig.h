#pragma once
/* ========================================================================
/*  include space
/* ===================================================================== */
#include <Data/Engine/Configs/Scene/Objects/Transform/UvTransformConfig.h>
#include <Data/Engine/Configs/Scene/Objects/Material/MaterialConfig.h>

struct BaseModelConfig {
    //========================= variable =========================
	MaterialConfig materialConfig;			//< マテリアル
	Transform2DConfig uvTransConfig;		//< 2Dトランスフォーム
	int blendMode = 5;						//< ブレンドモード(normal

};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(BaseModelConfig,
								   materialConfig,
								   uvTransConfig,
								   blendMode)
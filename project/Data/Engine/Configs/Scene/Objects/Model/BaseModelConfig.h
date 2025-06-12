#pragma once
/* ========================================================================
/*  include space
/* ===================================================================== */
#include <Data/Engine/Configs/Scene/Objects/Transform/UvTransformConfig.h>
#include <Data/Engine/Configs/Scene/Objects/Material/MaterialConfig.h>

#include <string>

struct BaseModelConfig {
	//========================= variable =========================
	MaterialConfig materialConfig;			//< マテリアル
	Transform2DConfig uvTransConfig;		//< 2Dトランスフォーム
	int blendMode = 5;						//< ブレンドモード(normal
	std::string modelName = "";				//< モデル名（ファイル名）

};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(BaseModelConfig,
								   materialConfig,
								   uvTransConfig,
								   blendMode,
								   modelName)
#pragma once

/* ========================================================================
/*  include space
/* ===================================================================== */
#include <Data/Engine/Configs/Scene/Objects/Collider/ColliderConfig.h>
#include <Data/Engine/Configs/Scene/Objects/Model/BaseModelConfig.h>
#include <Data/Engine/Configs/Scene/Objects/Transform/WorldTransformConfig.h>

#include <Engine/Foundation/Math/Vector3.h>

struct BaseGameObjectConfig {
    //========================= Collider =========================
	ColliderConfig colliderConfig;				//< コライダー設定
	BaseModelConfig modelConfig;				//< モデル設定
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(BaseGameObjectConfig,
								   colliderConfig,
								   modelConfig)
#pragma once
#include <Data/Engine/Macros/ConfigMacros/ConfigMacros.h>

#define APPLY_BASEMODEL_CONFIG        \
    materialData_.ApplyConfig;    \
    uvTransform.ApplyConfig();     \
    blendMode_ = static_cast<BlendMode>(config_.blendMode);

#define EXTRACT_BASEMODEL_CONFIG      \
    materialConfig.Extract();  \
    uvTransform.Extract();  \
    config_.blendMode = static_cast<int>(blendMode_);
#pragma once
#include <Data/Engine/Macros/ConfigMacros/ConfigMacros.h>

#define APPLY_WORLD_TRANSFORM_CONFIG      \
    CONFIG_APPLY_MEMBER(translation);     \
    CONFIG_APPLY_MEMBER(rotation);        \
    CONFIG_APPLY_MEMBER(scale);

#define EXTRACT_WORLD_TRANSFORM_CONFIG    \
    CONFIG_EXTRACT_MEMBER(translation);   \
    CONFIG_EXTRACT_MEMBER(rotation);      \
    CONFIG_EXTRACT_MEMBER(scale);
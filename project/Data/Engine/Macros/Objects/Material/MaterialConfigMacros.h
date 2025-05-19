#pragma once
#include <Data/Engine/Macros/ConfigMacros/ConfigMacros.h>

#define APPLY_MATERIAL_CONFIG           \
    CONFIG_APPLY_MEMBER(color);  \
    CONFIG_APPLY_MEMBER(enableLighting); \
    CONFIG_APPLY_MEMBER(shininess); \
    CONFIG_APPLY_MEMBER(enviromentCoefficient); \
    CONFIG_APPLY_MEMBER(isReflect); \
    CONFIG_APPLY_MEMBER(currentLightingMode_);

#define EXTRACT_MATERIAL_CONFIG         \
    CONFIG_EXTRACT_MEMBER(color);\
    CONFIG_EXTRACT_MEMBER(enableLighting); \
    CONFIG_EXTRACT_MEMBER(shininess); \
    CONFIG_EXTRACT_MEMBER(enviromentCoefficient); \
    CONFIG_EXTRACT_MEMBER(isReflect); \
    CONFIG_EXTRACT_MEMBER(currentLightingMode_); 
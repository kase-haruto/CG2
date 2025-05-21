#include "ConfigPathResolver.h"

std::string ConfigPathResolver::GetBaseDirectory() {
    return "Resources/Configs/Engine/Objects/";
}

std::string ConfigPathResolver::ResolvePath(const std::string& objectType,
                                            const std::string& objectName) {
    return GetBaseDirectory() + objectType + "/" + objectName + ".json";
}

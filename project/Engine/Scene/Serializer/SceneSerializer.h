#pragma once
/* ========================================================================
/*  include space
/* ===================================================================== */
#include <string>
#include <memory>
#include <vector>
#include <nlohmann/json.hpp>

class SceneContext;
class SceneObject;

class SceneSerializer {
public:
	static bool Save(const SceneContext& context, const std::string& path);
	static bool Load(SceneContext& context, const std::string& path);
};

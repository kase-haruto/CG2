#pragma once
/* ========================================================================
/*  include space
/* ===================================================================== */

#include <filesystem>

class SceneContext;

class SceneSerializer{
public:
	static bool Save(const SceneContext& ctx, const std::filesystem::path& file);
	static bool Load(SceneContext& ctx, const std::filesystem::path& file);

};


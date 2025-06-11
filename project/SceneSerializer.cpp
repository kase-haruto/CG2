#include "SceneSerializer.h"

/* ========================================================================
/*  include space
/* ===================================================================== */
#include <Engine/Foundation/Json/JsonFileIO.h>
#include <Engine/Scene/Context/SceneContext.h>
#include <Data/Engine/Configs/Scene/Objects/SceneObject/SceneObjectConfig.h>
#include <Data/Engine/Configs/Scene/Objects/BaseGameObject/BaseGameObjectConfig.h>

#include <vector>

//bool SceneSerializer::Save(const SceneContext& ctx, const std::filesystem::path& file){
//	std::vector<BaseGameObjectConfig> outConfigs;
//	for (auto* obj : ctx.GetObjectLibrary()->GetAllObjects()){
//		outConfigs.push_back(obj->ToConfig()); // virtual化されていれば派生型にも対応可能
//	}
//	return JsonFileIO::Write(file.string(), outConfigs); // 既存I/O活用
//}
//bool SceneSerializer::Load(SceneContext& ctx, const std::filesystem::path& file){
//	return false;
//}
//
//bool SceneSerializer::Load(SceneContext& ctx, const std::filesystem::path& file){
//	std::vector<BaseGameObjectConfig> configs;
//	if (!JsonFileIO::Read(file.string(), configs)) return false;
//
//	std::unordered_map<Guid, SceneObject*> guidMap;
//
//	// パス1: オブジェクト生成 + GUID登録
//	for (const auto& cfg : configs){
//		auto obj = std::make_unique<BaseGameObject>();
//		obj->FromConfig(cfg);
//		guidMap[cfg.guid] = obj.get();
//		ctx.GetObjectLibrary()->AddObject(std::move(obj));
//	}
//
//	// パス2: 親子関係を張る
//	for (const auto& cfg : configs){
//		if (cfg.parentGuid.isValid()){
//			auto* child = guidMap[cfg.guid];
//			auto* parent = guidMap[cfg.parentGuid];
//			parent->AddChild(child);
//		}
//	}
//
//	return true;
//}
#pragma once

/* ========================================================================
/*  include space
/* ===================================================================== */
#include <Data/Engine/Configs/Scene/Objects/Collider/ColliderConfig.h>

#include <Engine/Foundation/Math/Vector3.h>

struct Config final {
    //========================= Collider =========================
	ColliderConfig colliderConfig;				//< コライダー設定
	
	//========================= Transform =========================
	Vector3 translation = { 0.0f, 0.0f, 0.0f };	//< 位置
	Vector3 rotation = { 0.0f, 0.0f, 0.0f };	//< 回転
	Vector3 scale = { 1.0f, 1.0f, 1.0f };		//< スケール

	Vector3 uvScale = { 1.0f, 1.0f, 1.0f };		//< UVスケール
	Vector3 uvRotate = { 0.0f, 0.0f, 0.0f };	//< UV回転
	Vector3 uvTranslate = { 0.0f, 0.0f, 0.0f };	//< UVオフセット

	//========================= material =========================

};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Config, ex)
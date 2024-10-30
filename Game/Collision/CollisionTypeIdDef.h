#pragma once

#include<cstdint>

//コリジョン種別id定義
enum class CollisionTypeIdDef :uint32_t{
	kDefault,
	kBullet,
	kEnemy,
};
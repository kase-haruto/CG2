#pragma once

#include "lib/myMath/Vector3.h"
#include "lib/myMath/Quaternion.h"
#include "Engine/objects/Transform.h"

// C++
#include <vector>
#include <map>
#include <string>
#include <cstdint>
#include <optional>

/////////////////////////////////////////////////////////////////////////////////////////
//          keyframe
/////////////////////////////////////////////////////////////////////////////////////////
template <typename tValue>
struct Keyframe{
    float time;     // アニメーション時間(秒)
    tValue value;   // 補間対象の値 (Vector3 or Quaternion)
};
using KeyframeQuaternion = Keyframe<Quaternion>;
using KeyframeVector3 = Keyframe<Vector3>;

/////////////////////////////////////////////////////////////////////////////////////////
//          AnimationCurve
/////////////////////////////////////////////////////////////////////////////////////////
template<typename tValue>
struct AnimationCurve{
    std::vector<Keyframe<tValue>> keyframes;
};

/////////////////////////////////////////////////////////////////////////////////////////
//          NodeAnimation
/////////////////////////////////////////////////////////////////////////////////////////
struct NodeAnimation{
    AnimationCurve<Vector3>     translate;  // 平行移動
    AnimationCurve<Quaternion>  rotate;     // 回転
    AnimationCurve<Vector3>     scale;      // スケーリング
};

struct Node{
    QuaternionTransform transform;
	Matrix4x4 localMatrix;
	std::string name;
	std::vector<Node> children;
};

/////////////////////////////////////////////////////////////////////////////////////////
//          Animation
/////////////////////////////////////////////////////////////////////////////////////////
struct Animation{
    float duration = 0.0f;  // アニメーション全体の尺(秒)
    // ※本来は TicksPerSecond など、時間単位の情報もあると便利
    std::map<std::string, NodeAnimation> nodeAnimations;
};

/////////////////////////////////////////////////////////////////////////////////////////
//          skelton
/////////////////////////////////////////////////////////////////////////////////////////
struct Joint{
    QuaternionTransform transform;	//< transform情報
	Matrix4x4 localMatrix;			//< ローカル行列
	Matrix4x4 skeletonSpaceMatrix;	//< スケルトン空間行列
	std::string name;				//< ボーン名
	std::vector<int32_t> children;	//< 子ボーンのインデックス
	int32_t index;					//< インデックス
	std::optional<int32_t> parent;	//< 親ボーンのインデックス
};

struct Skeleton{
	int32_t root;								//< rootjointのインデックス
	std::map<std::string, int32_t> jointMap;	//< ボーン名とインデックスのマップ
	std::vector<Joint> joints;					//< 所属しているジョイント
};


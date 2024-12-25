#pragma once

#include "lib/myMath/Vector3.h"
#include "lib/myMath/Quaternion.h"

//*c++*//
#include <vector>
#include <map>

/////////////////////////////////////////////////////////////////////////////////////////
//					keyframe
/////////////////////////////////////////////////////////////////////////////////////////
template <typename tValue>
struct Keyframe{
	float time;
	tValue value;
};
using KeyframeQuaternion = Keyframe<Quaternion>;
using KeyframeVector3 = Keyframe<Vector3>;


/////////////////////////////////////////////////////////////////////////////////////////
//					NodeAnimation
/////////////////////////////////////////////////////////////////////////////////////////
template<typename tValue>
struct AnimationCurve{
	std::vector<Keyframe<tValue>>keyframes;
};

struct NodeAnimation{
	AnimationCurve<Vector3>translate;
	AnimationCurve<Quaternion>rotate;
	AnimationCurve<Vector3>scale;
};

struct Animation{
	float duration; //アニメーションの尺
	std::map<std::string, NodeAnimation>nodeAnimations;
};
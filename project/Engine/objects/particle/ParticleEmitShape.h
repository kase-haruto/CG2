#pragma once


#include "lib/myMath/Vector3.h"
#include "Engine/objects/Transform.h"


// ローカル空間で面を選択し、法線方向と位置を決定する関数例
struct FaceInfo{
    Vector3 localPoint;
    Vector3 localNormal;
};


FaceInfo GetRandomPointAndNormalOnOBBSurface(const Transform& transform,
                                             bool emitPosX, bool emitNegX,
                                             bool emitPosY, bool emitNegY,
                                             bool emitPosZ, bool emitNegZ);



Vector3 GetRandomPointOnSphere(const Transform& transform);
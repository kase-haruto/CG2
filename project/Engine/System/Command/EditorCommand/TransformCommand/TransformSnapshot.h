#pragma once

#include <Engine/Foundation/Math/Vector3.h>
#include <Engine/Foundation/Math/Quaternion.h>
#include <Engine/Objects/3D/Actor/SceneObject.h>


struct TransformSnapshot {
public:
    static TransformSnapshot Capture(const SceneObject* obj);
    void Apply(SceneObject* obj) const;

public:
    Vector3    scale;
    Quaternion rotate;
    Vector3    translate;
};

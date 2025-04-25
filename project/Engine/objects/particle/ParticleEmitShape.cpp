#include "ParticleEmitShape.h"

#include "lib/myFunc/Random.h"
#include "lib/myFunc/MyFunc.h"

#include <numbers>

FaceInfo GetRandomPointAndNormalOnOBBSurface(const EulerTransform& transform,
                                             bool emitPosX, bool emitNegX,
                                             bool emitPosY, bool emitNegY,
                                             bool emitPosZ, bool emitNegZ){
    Vector3 halfExtents = transform.scale * 0.5f;

    // 有効な面をリストアップ
    std::vector<int> enabledFaces;
    if (emitPosX) enabledFaces.push_back(0); // +X
    if (emitNegX) enabledFaces.push_back(1); // -X
    if (emitPosY) enabledFaces.push_back(2); // +Y
    if (emitNegY) enabledFaces.push_back(3); // -Y
    if (emitPosZ) enabledFaces.push_back(4); // +Z
    if (emitNegZ) enabledFaces.push_back(5); // -Z

    // 有効な面が無い場合はデフォルトで+X面などにフォールバックするか、
    // 何もしないようにするなどの対処が必要
    if (enabledFaces.empty()){
        enabledFaces.push_back(0); // fallback
    }

    // 有効面からランダムに1つ選ぶ
    int faceIndex = enabledFaces[Random::Generate(0, static_cast< int >(enabledFaces.size()) - 1)];

    Vector3 localPoint {};
    Vector3 localNormal {};
    switch (faceIndex){
        case 0: // +X面
            localPoint = {+halfExtents.x,
                          Random::Generate(-halfExtents.y, halfExtents.y),
                          Random::Generate(-halfExtents.z, halfExtents.z)};
            localNormal = {1.0f, 0.0f, 0.0f};
            break;
        case 1: // -X面
            localPoint = {-halfExtents.x,
                          Random::Generate(-halfExtents.y, halfExtents.y),
                          Random::Generate(-halfExtents.z, halfExtents.z)};
            localNormal = {-1.0f, 0.0f, 0.0f};
            break;
        case 2: // +Y面
            localPoint = {Random::Generate(-halfExtents.x, halfExtents.x),
                          +halfExtents.y,
                          Random::Generate(-halfExtents.z, halfExtents.z)};
            localNormal = {0.0f, 1.0f, 0.0f};
            break;
        case 3: // -Y面
            localPoint = {Random::Generate(-halfExtents.x, halfExtents.x),
                          -halfExtents.y,
                          Random::Generate(-halfExtents.z, halfExtents.z)};
            localNormal = {0.0f, -1.0f, 0.0f};
            break;
        case 4: // +Z面
            localPoint = {Random::Generate(-halfExtents.x, halfExtents.x),
                          Random::Generate(-halfExtents.y, halfExtents.y),
                          +halfExtents.z};
            localNormal = {0.0f, 0.0f, 1.0f};
            break;
        case 5: // -Z面
            localPoint = {Random::Generate(-halfExtents.x, halfExtents.x),
                          Random::Generate(-halfExtents.y, halfExtents.y),
                          -halfExtents.z};
            localNormal = {0.0f, 0.0f, -1.0f};
            break;
    }

    return {localPoint, localNormal};
}

Vector3 GetRandomPointOnSphere(const EulerTransform& transform){
    float radius = transform.scale.x * 0.5f;

    float theta = Random::Generate(0.0f, float(std::numbers::pi)*2);
    float phi = std::acos(Random::Generate(-1.0f, 1.0f));

    float sinPhi = std::sin(phi);
    float x = radius * sinPhi * std::cos(theta);
    float y = radius * sinPhi * std::sin(theta);
    float z = radius * std::cos(phi);

    return  {x, y, z};
}
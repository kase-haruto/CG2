#include "Shape.h"

#include "lib/myFunc/PrimitiveDrawer.h"
#include "lib/myFunc/myfunc.h"

#include <numbers>

std::array<Vector3, 8> OBB::GetVertices() const{
    // ローカル座標系の頂点オフセット
    std::array<Vector3, 8> localVertices = {
        Vector3(-size.x, -size.y, -size.z),
        Vector3(-size.x, -size.y,  size.z),
        Vector3(-size.x,  size.y, -size.z),
        Vector3(-size.x,  size.y,  size.z),
        Vector3(size.x, -size.y, -size.z),
        Vector3(size.x, -size.y,  size.z),
        Vector3(size.x,  size.y, -size.z),
        Vector3(size.x,  size.y,  size.z)
    };

    // 回転行列を計算
    Matrix4x4 rotateX = MakeRotateXMatrix(rotate.x);
    Matrix4x4 rotateY = MakeRotateYMatrix(rotate.y);
    Matrix4x4 rotateZ = MakeRotateZMatrix(rotate.z);
    Matrix4x4 rotationMatrix = Matrix4x4::Multiply(Matrix4x4::Multiply(rotateX,rotateY), rotateZ);

    // グローバル座標系での頂点位置を計算
    std::array<Vector3, 8> globalVertices;
    for (size_t i = 0; i < localVertices.size(); ++i){
        globalVertices[i] = Vector3::Transform(localVertices[i], rotationMatrix) + center;
    }

    return globalVertices;
}


void OBB::Draw(){
    const uint32_t vertexNum = 8;

    // 回転行列を計算
    Matrix4x4 rotationMatrix = EulerToMatrix(rotate);

    // 各軸の半サイズを回転
    Vector3 halfSizeX = Vector3::Transform({1.0f, 0.0f, 0.0f}, rotationMatrix) * size.x * 0.5f;
    Vector3 halfSizeY = Vector3::Transform({0.0f, 1.0f, 0.0f}, rotationMatrix) * size.y * 0.5f;
    Vector3 halfSizeZ = Vector3::Transform({0.0f, 0.0f, 1.0f}, rotationMatrix) * size.z * 0.5f;

    // 頂点を計算
    Vector3 vertices[vertexNum];
    Vector3 offsets[vertexNum] = {
        {-1, -1, -1}, {-1,  1, -1}, {1, -1, -1}, {1,  1, -1},
        {-1, -1,  1}, {-1,  1,  1}, {1, -1,  1}, {1,  1,  1}
    };

    for (int i = 0; i < vertexNum; ++i){
        Vector3 localVertex = offsets[i].x * halfSizeX +
            offsets[i].y * halfSizeY +
            offsets[i].z * halfSizeZ;
        vertices[i] = center + localVertex;
    }

    // 辺を描画
    int edges[12][2] = {
        {0, 1}, {1, 3}, {3, 2}, {2, 0},
        {4, 5}, {5, 7}, {7, 6}, {6, 4},
        {0, 4}, {1, 5}, {2, 6}, {3, 7}
    };

    for (int i = 0; i < 12; ++i){
        int start = edges[i][0];
        int end = edges[i][1];
        PrimitiveDrawer::GetInstance()->DrawLine3d(vertices[start], vertices[end], {1.0f,0.0f,0.0f,1.0f});
    }
}

void Sphere::Draw(){

    // 分割数
    const uint32_t kSubdivision = 16; // 任意の適切な値を設定
    // 軽度分割1つ分の角度
    const float kLonEvery = 2 * float(std::numbers::pi) / kSubdivision;
    // 緯度分割1つ分の角度
    const float kLatEvery = float(std::numbers::pi) / kSubdivision;
    Vector3 a, b, c;
    
    //緯度の方向に分割　-n/2 ~ n/2
    for (uint32_t latIndex = 0; latIndex < kSubdivision; ++latIndex){
        float lat = -float(std::numbers::pi) / 2.0f + kLatEvery * latIndex;
        //軽度の方向に分割 0~2π
        for (uint32_t lonIndex = 0; lonIndex < kSubdivision; ++lonIndex){
            float lon = lonIndex * kLonEvery;

            // 点の座標を計算
            a.x = radius * (std::cos(lat) * std::cos(lon)) + center.x;
            a.y = radius * std::sin(lat) + center.y;
            a.z = radius * (std::cos(lat) * std::sin(lon)) + center.z;

            b.x = radius * (std::cos(lat + kLatEvery) * std::cos(lon)) + center.x;
            b.y = radius * std::sin(lat + kLatEvery) + center.y;
            b.z = radius * (std::cos(lat + kLatEvery) * std::sin(lon)) + center.z;

            c.x = radius * (std::cos(lat) * std::cos(lon + kLonEvery)) + center.x;
            c.y = radius * std::sin(lat) + center.y;
            c.z = radius * (std::cos(lat) * std::sin(lon + kLonEvery)) + center.z;

            //ab,bcで線を引く
            PrimitiveDrawer::GetInstance()->DrawLine3d(a, b, {1.0f,0.0f,0.0f,1.0f});
            PrimitiveDrawer::GetInstance()->DrawLine3d(b, c, {1.0f,0.0f,0.0f,1.0f});
        }
    }

}
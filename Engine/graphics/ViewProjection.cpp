#include "ViewProjection.h"
#include <cmath>
#include "DirectXCommon.h"
#include <imgui.h>
#include "myFunc/MyFunc.h"
#include "GraphicsGroup.h"

ViewProjection::ViewProjection(){
    commandList_ = GraphicsGroup::GetInstance()->GetCommandList();
    rootSignature_ = GraphicsGroup::GetInstance()->GetRootSignature(Object3D);
    device_ = GraphicsGroup::GetInstance()->GetDevice();
}

ViewProjection::~ViewProjection(){}

void ViewProjection::CreateConstBuffer(){
    constBuffer_ = CreateBufferResource(device_, sizeof(CameraForGPU));
}

void ViewProjection::Map(){
    constBuffer_->Map(0, nullptr, reinterpret_cast< void** >(&cameraData_));
    cameraData_->worldPosition = transform.translate;
    constBuffer_->Unmap(0, nullptr);  // データ設定後にアンマップ
}

void ViewProjection::Initialize(){
    // 定数バッファの生成
    CreateConstBuffer();
    Map();

    cameraMatrix = MakeAffineMatrix(transform.scale,
                                    transform.rotate,
                                    transform.translate
    );

    matView = Matrix4x4::Inverse(cameraMatrix);
    matProjection = MakePerspectiveFovMatrix(fovAngleY, aspectRatio, nearZ, farZ);
    viewProjection_ = Matrix4x4::Multiply(matView, matProjection);
    UpdateMatrix();
}

void ViewProjection::UpdateMatrix(){
    UpdateViewMatrix();
    UpdateProjectionMatrix();
    viewProjection_ = Matrix4x4::Multiply(matView, matProjection);
}

void ViewProjection::UpdateViewMatrix(){
    Matrix4x4 cameraMatrix = MakeAffineMatrix(transform.scale,
                                              transform.rotate,
                                              transform.translate
    );
    matView = Matrix4x4::Inverse(cameraMatrix);
    commandList_->SetGraphicsRootSignature(rootSignature_.Get());
    commandList_->SetGraphicsRootConstantBufferView(5, constBuffer_->GetGPUVirtualAddress());
}

void ViewProjection::UpdateProjectionMatrix(){
    matProjection = MakePerspectiveFovMatrix(fovAngleY, aspectRatio, nearZ, farZ);
}

Vector3 ViewProjection::Unproject(const Vector2& screenPos) const{
    // ビューポート行列の作成
    Matrix4x4 matViewport = Matrix4x4::MakeViewportMatrix(0, 0, 1280.0f, 720.0f, 0.0f, 1.0f);

    // ビュープロジェクション行列の合成
    Matrix4x4 matVP = Matrix4x4::Multiply(matView, matProjection);

    // 合成行列の逆行列を取得
    Matrix4x4 matInverseVP = Matrix4x4::Inverse(matVP);

    // 2Dスクリーン座標の正規化（[0, 1] → [-1, 1]）
    float normalizedX = (2.0f * screenPos.x / 1280.0f) - 1.0f;
    float normalizedY = 1.0f - (2.0f * screenPos.y / 720.0f); // DirectXではY軸が反転

    // 射影面上のZ値を取得（0.0f から 1.0f で指定可能）
    Vector3 posNear = {normalizedX, normalizedY, 0.0f}; // 近くの平面
    Vector3 posFar = {normalizedX, normalizedY, 1.0f};  // 遠くの平面

    // 近くの平面と遠くの平面をワールド座標系に変換
    posNear = Matrix4x4::Transform(posNear, matInverseVP);
    posFar = Matrix4x4::Transform(posFar, matInverseVP);

    // 近くの点と遠くの点から方向を計算
    Vector3 direction = posFar - posNear;
    direction.Normalize();

    // カメラからのレイを計算し、射影面での交点を取得
    float distanceToProjectionPlane = -posNear.z / direction.z;
    Vector3 intersectionPoint = posNear + (direction * distanceToProjectionPlane);

    return intersectionPoint;
}




Matrix4x4 ViewProjection::MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip){
    Matrix4x4 result;
    result = {
        1 / (aspectRatio * std::tan(fovY / 2)), 0, 0, 0,
        0, 1 / std::tan(fovY / 2), 0, 0,
        0, 0, farClip / (farClip - nearClip), 1,
        0, 0, -nearClip * farClip / (farClip - nearClip), 0
    };
    return result;
}

void ViewProjection::ImGui(){
    ImGui::Begin("camera");
    ImGui::DragFloat3("pos", &transform.translate.x, 0.01f);
    ImGui::DragFloat3("rotate", &transform.rotate.x, 0.01f);
    ImGui::End();
    cameraData_->worldPosition = transform.translate;

}

void ViewProjection::Finalize(){}

#include "ViewProjection.h"
#include <cmath>
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
#ifdef _DEBUG
    ImGui::Begin("camera");
    ImGui::DragFloat3("pos", &transform.translate.x, 0.01f);
    ImGui::DragFloat3("rotate", &transform.rotate.x, 0.01f);
    ImGui::End();
    cameraData_->worldPosition = transform.translate;
#endif // _DEBUG
}

void ViewProjection::Finalize(){}

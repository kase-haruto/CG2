#include "Engine/graphics/camera/Camera3d.h"

/* engine */
#include "lib/myFunc/MyFunc.h"
#include "engine/graphics/GraphicsGroup.h"

/* c++ */
#include <cmath>

/* externals */
#include <externals/imgui/imgui.h>

Camera3d::Camera3d()
    :worldMatrix_(MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate))
    , viewMatrix_(Matrix4x4::Inverse(worldMatrix_))
    , projectionMatrix_(MakePerspectiveFovMatrix(fovAngleY_, aspectRatio_, nearZ_, farZ_))
    , viewProjectionMatrix_(Matrix4x4::Multiply(viewMatrix_, projectionMatrix_))
{
    device_ = GraphicsGroup::GetInstance()->GetDevice();
    commandList_ = GraphicsGroup::GetInstance()->GetCommandList();
    rootSignature_ = GraphicsGroup::GetInstance()->GetRootSignature(Object3D);

    CreateBuffer();
    Map();
}

void Camera3d::Update(){
   
    /////////////////////////////////////////////////////////////////////////////////////////
    //      行列の更新
    /////////////////////////////////////////////////////////////////////////////////////////
    worldMatrix_ = MakeAffineMatrix(transform_.scale,
                                    transform_.rotate,
                                    transform_.translate
    );

    viewMatrix_ = Matrix4x4::Inverse(worldMatrix_);
    projectionMatrix_ = MakePerspectiveFovMatrix(fovAngleY_, aspectRatio_, nearZ_, farZ_);

    viewProjectionMatrix_ = Matrix4x4::Multiply(viewMatrix_, projectionMatrix_);

    commandList_->SetGraphicsRootSignature(rootSignature_.Get());
    commandList_->SetGraphicsRootConstantBufferView(5, constBuffer_->GetGPUVirtualAddress());
}

/////////////////////////////////////////////////////////////////////////////////////////
/*                                       buffer の生成                                 */
/////////////////////////////////////////////////////////////////////////////////////////

void Camera3d::CreateBuffer(){
    constBuffer_ = CreateBufferResource(device_, sizeof(CameraForGPU));
}

void Camera3d::Map(){
    constBuffer_->Map(0, nullptr, reinterpret_cast< void** >(&cameraData_));
    cameraData_->worldPosition = transform_.translate;
    constBuffer_->Unmap(0, nullptr);  // データ設定後にアンマップ
}

Matrix4x4 Camera3d::MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip){
    Matrix4x4 result;
    result = {
        1 / (aspectRatio * std::tan(fovY / 2)), 0, 0, 0,
        0, 1 / std::tan(fovY / 2), 0, 0,
        0, 0, farClip / (farClip - nearClip), 1,
        0, 0, -nearClip * farClip / (farClip - nearClip), 0
    };
    return result;
}
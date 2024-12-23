#pragma once

#include "Engine/objects/Transform.h"

//* math *//
#include "lib/myMath/Matrix4x4.h"

//* c++ *//
#include <numbers>
#include <d3d12.h>
#include <wrl.h>

using namespace Microsoft::WRL;

// 定数バッファ用data構造体
struct Camera3dForGPU{
    Vector3 worldPosition;
    float padding;
};

class Camera3d{
public:
    Camera3d();
    ~Camera3d() = default;

    void Update();  //更新

    void SetCamera(const Vector3& pos, const Vector3& rotate);

private:
    /////////////////////////////////////////////////////////////////////////////////////////
    /*                                       buffer                                  */
   /////////////////////////////////////////////////////////////////////////////////////////
    void CreateBuffer();

    void Map();

private:
    Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip);


private:
    Transform transform_ = {
        {1.0f, 1.0f, 1.0f},         // scale
        {0.0f, 0.0f, 0.0f},         // rotate
        {0.0f, 4.0f, -15.0f}        // translate
    };
    Matrix4x4 worldMatrix_;         // ワールド行列
    Matrix4x4 viewMatrix_;          // ビュー行列
    Matrix4x4 projectionMatrix_;    // projection
    Matrix4x4 viewProjectionMatrix_;// viewProjection


private:
    float aspectRatio_ = ( float ) 16 / 9;                        // ビューポートのアスペクト比
    float nearZ_ = 0.1f;                                          // 深度限界（手前側）
    float farZ_ = 1000.0f;                                        // 深度限界（奥側）
    float fovAngleY_ = 45.0f * float(std::numbers::pi) / 180.0f;  // 垂直方向視野角


private:

    ComPtr<ID3D12Resource> constBuffer_;         // 定数バッファ
    Camera3dForGPU* cameraData_;                 // カメラのデータ

    // DirectX関連
    ComPtr<ID3D12Device> device_ = nullptr;
    ComPtr<ID3D12GraphicsCommandList> commandList_ = nullptr;
    ComPtr<ID3D12RootSignature>rootSignature_ = nullptr;



    /////////////////////////////////////////////////////////////////////////////////////////
    /*                                       アクセッサ関数                                  */
    /////////////////////////////////////////////////////////////////////////////////////////
public:
    //setter
    void SetRotate(const Vector3& rotate){ transform_.rotate = rotate; }
    void SetTranslate(const Vector3& translate){ transform_.translate = translate; }
    void SetFovY(const float fov){ fovAngleY_ = fov; }
    void SetAspectRatio(const float aspect){ aspectRatio_ = aspect; }
    //void SetNearClip(const float nearZ){ nearZ_ = near; }
    //void SetFarClip(const float far){ farZ_ = far; }

    // getter
    const Matrix4x4& GetWorldMat()const{ return worldMatrix_; }
    const Matrix4x4& GetViewMatrix()const{ return viewMatrix_; }
    const Matrix4x4& GetProjectionMatrix()const{ return projectionMatrix_; }
    const Matrix4x4& GetViewProjectionMatrix()const{ return viewProjectionMatrix_; }
    const Vector3& GetRotate()const{ return transform_.rotate; }
    const Vector3& GetTranslate()const{ return transform_.translate; }

};

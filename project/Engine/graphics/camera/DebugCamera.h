#pragma once

#include "BaseCamera.h"
#include "lib/myMath/Vector3.h"
#include "lib/myMath/Vector2.h"

// <summary>
// Blenderライクな操作のデバッグカメラ
// - MMB: 回転(Orbit)
// - Shift + MMB: パン
// - Ctrl + MMB or マウスホイール: ズーム
// </summary>
class DebugCamera
    : public BaseCamera{
public:
    //===================================================================//
    //							public func
    //===================================================================//
    DebugCamera();
    ~DebugCamera() = default;

    void Update() override;

    //===================================================================//
	//							getter / setter
    //===================================================================//
    //* ターゲット（注視点）を設定
    void SetTarget(const Vector3& target){ target_ = target; }

    //* カメラとターゲットとの初期距離を設定
    void SetDistance(float dist){ distance_ = dist; }

private:
    //===================================================================//
    //							private func
    //===================================================================//
    void Move();    //< パン処理
    void Rotate();  //< 回転処理(Orbit)
    void Zoom();    //< ズーム処理

    //===================================================================//
	//							private variables
    //===================================================================//
    Vector3 target_ {0.0f, 0.0f, 0.0f};    //* ターゲット（注視点）
    float distance_ = 10.0f;               //* カメラまでの距離
    Vector2 orbitAngle_ {0.0f, 0.0f};      //* オービット時の回転角度(Yaw, Pitch)
	Vector2 lastMousePos_ {0.0f, 0.0f};    //* 前フレームのマウス座標
    // 操作速度
    float rotateSpeed_ = 0.005f;    //* 回転速度
    float panSpeed_ = 0.01f;        //* パンスピード
    float zoomSpeed_ = 0.1f;        //* ズーム速度
};

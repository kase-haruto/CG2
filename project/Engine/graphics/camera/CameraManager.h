#pragma once

#include "Engine/graphics/camera/Camera3d.h"
#include "Engine/graphics/camera/FollowCamera.h"
#include "Engine/graphics/camera/DebugCamera.h"
#include "Engine/graphics/camera/BaseCamera.h"

//* c++ *//
#include <memory>
#include <unordered_map>

enum CameraType{
    Type_Default,
    Type_Follow,
    Type_Debug,
};

class CameraManager{
public:
    // インスタンスを取得する関数
    static CameraManager* GetInstance();

    // コピーコンストラクタと代入演算子を削除して、複製を防止
    CameraManager(const CameraManager&) = delete;
    CameraManager& operator=(const CameraManager&) = delete;

public:
    //===================================================================*/
    //					private member function
    //===================================================================*/
    static void Initialize();      // 初期化
    static void Update();          // 更新
    static void Finalize();        // 解放

	static void TransfarToGPU();   // GPUへの転送

	static void SetCommand(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> command, PipelineType pipelineType){
		instance_->cameras_[instance_->type_]->SetCommand(command, pipelineType);
	}

private:
    CameraManager();
    ~CameraManager() = default;

private:
    //===================================================================*/
    //					private member variable
    //===================================================================*/
    static CameraManager* instance_;                 // クラスのインスタンス
    CameraType type_ = Type_Default;

    /* 管理しているカメラ =======================*/
    std::unique_ptr<Camera3d> camera3d_ = nullptr;   // 3dオブジェクトのデフォルトカメラ
    std::unique_ptr<FollowCamera> followCamera_ = nullptr;
	std::unique_ptr<DebugCamera> debugCamera_ = nullptr;

    // すべてのカメラ
    std::unordered_map<CameraType, BaseCamera*> cameras_;

    //===================================================================*/
    //					getter/setter
    //===================================================================*/
public:
    static Camera3d* GetCamera3d(){ return instance_->camera3d_.get(); }

	static DebugCamera* GetDebugCamera() { return instance_->debugCamera_.get(); }

    static BaseCamera* GetActiveCamera(){ return instance_->cameras_[instance_->type_]; }

	static Matrix4x4 GetViewProjectionMatrix(){ return instance_->cameras_[instance_->type_]->GetViewProjection(); }

    static Matrix4x4 GetViewMatrix(){ return instance_->cameras_[instance_->type_]->GetViewMatrix(); }

	static Matrix4x4 GetWorldMatrix(){ return instance_->cameras_[instance_->type_]->GetWorldMat(); }

    void SetFollowTarget(const EulerTransform* target){
        followCamera_->SetTarget(target); 
    }

    const Vector3& GetFollowRotate()const{ return followCamera_->GetRotate(); }

	void SetRotate(const Vector3& rotate){
		followCamera_->SetRotate(rotate);
	}

    void SetType(const CameraType type);

	void SetShake(float duration, float intensity){
		instance_->cameras_[instance_->type_]->StartShake(duration, intensity);
	}
};
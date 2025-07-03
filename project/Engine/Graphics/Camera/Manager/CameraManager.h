#pragma once
/* ========================================================================
/*	include space
/* ===================================================================== */
// engine
#include <Engine/Graphics/Camera/3d/Camera3d.h>
#include <Engine/graphics/camera/3d/FollowCamera.h>
#include <Engine/graphics/camera/3d/DebugCamera.h>
#include <Engine/graphics/camera/Base/BaseCamera.h>
#include <Engine/Graphics/Camera/Viewport/ViewportDetail.h>

//* c++ *//
#include <memory>
#include <unordered_map>

/* ========================================================================
/*	enum
/* ===================================================================== */
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

	void SetAspectRatio(float width, float height);

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

	Vector2 mainViewportSize_{ 1920.0f, 1080.0f };
	Vector2 debugViewportSize_{ 800.0f, 600.0f };

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

	static DebugCamera* GetDebugCamera(){ return instance_->debugCamera_.get(); }

	static BaseCamera* GetActiveCamera(){ return instance_->cameras_[instance_->type_]; }

	static Matrix4x4 GetViewProjectionMatrix(){ return instance_->cameras_[instance_->type_]->GetViewProjectionMatrix(); }

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

	void SetViewportSize(ViewportType type, const Vector2& size);
	Vector2 GetViewportSize(ViewportType type) const;
};
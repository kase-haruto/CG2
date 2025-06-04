#pragma once
/* ========================================================================
/* include
/* ===================================================================== */
#include <Engine/Graphics/Camera/3d/Camera3d.h>
#include <Engine/Objects/Transform/Transform.h>

class RailCamera :
	public Camera3d{
public:
	//===================================================================*/
	//		public functions
	//===================================================================*/

	RailCamera();
	~RailCamera() = default;

	void Initialize();
	void Update() override;
	void DrawRail();

	Vector3 GetPosition() ;

	const Vector3& GetRotation() const{
		return transform_.rotate;
	}

	const WorldTransform& GetWorldTransform()const{ return worldTransform_; }

	//--------- accessor --------------------------------------------------

private:
	float t_;
	Vector3 scrollVal_;
	std::vector<Vector3> railPoints_;
	WorldTransform worldTransform_;
	float zTiltOffset_ = 0.0f;    // 現在の傾き
	float targetTilt_ = 0.0f;     // 目標の傾き
	float tiltAngle_ = 0.05f;     // 最大角度（ラジアン）
	float tiltLerpSpeed_ = 10.0f; // 補間の速さ（大きいほど速く追従）

};


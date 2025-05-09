#pragma once
/* ========================================================================
/* include
/* ===================================================================== */
#include <Engine/graphics/camera/Camera3d.h>
#include <Engine/objects/Transform.h>

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

};


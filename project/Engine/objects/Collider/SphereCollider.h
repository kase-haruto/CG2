#pragma once

//* engine
#include "Collider.h"

class SphereCollider :
	public Collider{
public:
	//===================================================================*/
	//                   public methods
	//===================================================================*/
	SphereCollider() = default;
	~SphereCollider()override = default;

	void Initialize(float radius);
	void Update(const Vector3& position, const Quaternion& rotate)override;
	void Draw()override;
	void ShowGui()override;

	//* collision ==========================================*//
	void OnCollisionEnter([[maybe_unused]] Collider* other)override{};
	void OnCollisionStay([[maybe_unused]] Collider* other)override{};
	void OnCollisionExit([[maybe_unused]] Collider* other)override{};

protected:
	//===================================================================*/
	//                   private methods
	//===================================================================*/
	Sphere shape_; //衝突判定用のobb

public:
	//===================================================================*/
	//                   getter/setter
	//===================================================================*/
	const Vector3& GetCenter()const override;

	const std::variant<Sphere, OBB>& GetCollisionShape() override;
};

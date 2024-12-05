#include "SphereCollider.h"

void SphereCollider::Initialize(float radius){

	collisionShape_ = Sphere {shape_};
	shape_.radius = radius;

}

void SphereCollider::Draw(){

	shape_.Draw();
}

const Vector3& SphereCollider::GetCenter() const{
	return shape_.center;
}

const std::variant<Sphere, OBB>& SphereCollider::GetCollisionShape(){
	collisionShape_ = shape_;
	return collisionShape_;
};

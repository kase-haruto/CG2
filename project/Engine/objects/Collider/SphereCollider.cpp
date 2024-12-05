#include "SphereCollider.h"

#include <sstream> 

void SphereCollider::Initialize(float radius){

	std::stringstream ss;
	ss << "sphere" << "_" << this; // 形状とアドレスを組み合わせ
	name_ = ss.str();

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

#include "BoxCollider.h"
#include "lib/myFunc/PrimitiveDrawer.h"

#include <sstream> 

void BoxCollider::Initialize(const Vector3& size){
	std::stringstream ss;
	ss << "box" << "_" << this; // 形状とアドレスを組み合わせ
	name_ = ss.str();

	collisionShape_ = shape_;

	shape_.size = size;
}

void BoxCollider::Draw(){

#ifdef _DEBUG
	shape_.Draw();
#endif // DEBUG

}

const Vector3& BoxCollider::GetCenter() const{
	return shape_.center;
}

const std::variant<Sphere, OBB>& BoxCollider::GetCollisionShape() {
	collisionShape_ = shape_;
	return collisionShape_;
};
#include "BoxCollider.h"
#include "lib/myFunc/PrimitiveDrawer.h"

void BoxCollider::Initialize(const Vector3& size){
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
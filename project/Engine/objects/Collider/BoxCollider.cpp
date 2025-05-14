#include "BoxCollider.h"
/* ========================================================================
/*	include space
/* ===================================================================== */
// engine
#include <Engine/Renderer/Primitive/PrimitiveDrawer.h>
#include <Engine/Foundation/Json/JsonCoordinator.h>

// externals
#include <externals/imgui/imgui.h>
// c++
#include <sstream> 

void BoxCollider::Update(const Vector3& position, const Quaternion& rotate){
	// 位置を更新
	shape_.center = position;
	shape_.rotate = rotate;
}

void BoxCollider::Initialize([[maybe_unused]]const Vector3& size){

	if (name_.empty()){
		std::stringstream ss;
		ss << "box" << "_" << this; // 形状とアドレスを組み合わせ
		name_ = ss.str();
	}

	//JsonCoordinator::RegisterItem(name_, "ColliderSize", shape_.size);

	collisionShape_ = shape_;
	shape_.size = size;
}

void BoxCollider::Draw(){

#ifdef _DEBUG

	PrimitiveDrawer::GetInstance()->DrawOBB(shape_.center,shape_.rotate,shape_.size, color_);
#endif // DEBUG

}

void BoxCollider::ShowGui(){

#ifdef _DEBUG
	if (ImGui::CollapsingHeader(GetName().c_str())){

	}
	ImGui::Spacing();
#endif // _DEBUG


}

const Vector3& BoxCollider::GetCenter() const{
	return shape_.center;
}

const std::variant<Sphere, OBB>& BoxCollider::GetCollisionShape() {
	collisionShape_ = shape_;
	return collisionShape_;
};
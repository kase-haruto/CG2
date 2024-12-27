#include "BoxCollider.h"
#include "lib/myFunc/PrimitiveDrawer.h"

#include "engine/core/Json/JsonCoordinator.h"

#include <externals/imgui/imgui.h>

#include <sstream> 

void BoxCollider::Initialize([[maybe_unused]]const Vector3& size){

	if (name_.empty()){
		std::stringstream ss;
		ss << "box" << "_" << this; // 形状とアドレスを組み合わせ
		name_ = ss.str();
	}

	JsonCoordinator::RegisterItem(name_, "ColliderSize", shape_.size);

	collisionShape_ = shape_;

	//shape_.size = size;

	jsonPath = "gameObjects/";

	JsonCoordinator::LoadGroup(name_,jsonPath);

}

void BoxCollider::Draw(){

#ifdef _DEBUG
	shape_.Draw();
#endif // DEBUG

}

void BoxCollider::ShowGui(){

#ifdef _DEBUG

	std::string headerName = GetName() + "Collider";
	if (ImGui::CollapsingHeader(headerName.c_str())){
		JsonCoordinator::RenderGroupUI(name_);
	}

#endif // _DEBUG


}

const Vector3& BoxCollider::GetCenter() const{
	return shape_.center;
}

const std::variant<Sphere, OBB>& BoxCollider::GetCollisionShape() {
	collisionShape_ = shape_;
	return collisionShape_;
};
#include "BoxCollider.h"
#include "lib/myFunc/PrimitiveDrawer.h"

#include "engine/core/Json/JsonCoordinator.h"

#include <externals/imgui/imgui.h>

#include <sstream> 

void BoxCollider::Initialize(const Vector3& size){
	std::stringstream ss;
	ss << "box" << "_" << this; // 形状とアドレスを組み合わせ
	name_ = ss.str();

	JsonCoordinator::RegisterItem(name_, "ColliderSize", shape_.size);

	collisionShape_ = shape_;

	shape_.size = size;

	jsonPath = "gameobject/" + GetName();

}

void BoxCollider::Draw(){

#ifdef _DEBUG
	shape_.Draw();
#endif // DEBUG

}

void BoxCollider::ShowGui(){

#ifdef _DEBUG

	if (ImGui::CollapsingHeader("Collider")){
		if (ImGui::Button("save")){
			JsonCoordinator::Save(name_, jsonPath);
		}
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
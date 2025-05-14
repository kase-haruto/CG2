#include "SphereCollider.h"
#include <Engine/Renderer/Primitive/PrimitiveDrawer.h>

#include <engine/core/Json/JsonCoordinator.h>
#include <externals/imgui/imgui.h>

#include <sstream> 

void SphereCollider::Initialize(float radius){

	std::stringstream ss;
	ss << "sphere" << "_" << this; // 形状とアドレスを組み合わせ
	name_ = ss.str();

	JsonCoordinator::RegisterItem(name_, "ColliderRadius", shape_.radius);

	collisionShape_ = Sphere {shape_};
	shape_.radius = radius;

}

void SphereCollider::Draw(){

#ifdef _DEBUG
	shape_.Draw();
#endif // DEBUG

}

void SphereCollider::ShowGui(){

#ifdef _DEBUG
	ImGui::Spacing(); // 少しスペースを追加
	std::string headerName = GetName() + "Collider";
	if (ImGui::CollapsingHeader(headerName.c_str())){
		JsonCoordinator::RenderGroupUI(name_);
	}
	ImGui::Spacing();
#endif // _DEBUG


}

const Vector3& SphereCollider::GetCenter() const{
	return shape_.center;
}

const std::variant<Sphere, OBB>& SphereCollider::GetCollisionShape(){
	collisionShape_ = shape_;
	return collisionShape_;
};

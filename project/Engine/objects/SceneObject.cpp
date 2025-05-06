#include "SceneObject.h"
#include <externals/imgui/imgui.h>
#include "SceneObjectManager.h"

SceneObject::~SceneObject(){
	SceneObjectManager::GetInstance()->RemoveObject(this);
}

SceneObject::SceneObject(){

	//リストに追加
	SceneObjectManager::GetInstance()->AddObject(this);

}

void SceneObject::ShowGui(){
	ImGui::Dummy(ImVec2(0.0f, 5.0f));
	ImGui::Text("Name : %s", name_.c_str());
	ImGui::Dummy(ImVec2(0.0f, 5.0f));
	ImGui::Separator();
}

void SceneObject::EnableGuiList() {
	//リストに追加
	SceneObjectManager::GetInstance()->AddObject(this);
}

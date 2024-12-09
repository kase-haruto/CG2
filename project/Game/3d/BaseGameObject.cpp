#include "BaseGameObject.h"

#include "externals/imgui/imgui.h"
#include "Engine/core/Json/JsonCoordinator.h"


// 静的メンバ変数の定義
std::vector<BaseGameObject*> BaseGameObject::allGameObjects_;

BaseGameObject::BaseGameObject(const std::string& modelName){

	model_ = std::make_unique<Model>(modelName);
	model_->Initialize();

}

BaseGameObject::~BaseGameObject(){



}


void BaseGameObject::Initialize(){

	// JsonCoordinator にプロパティを登録
	JsonCoordinator::RegisterItem(objectName_, "Scale", model_->transform.scale);
	JsonCoordinator::RegisterItem(objectName_, "Rotate", model_->transform.rotate);
	JsonCoordinator::RegisterItem(objectName_, "Translate", model_->transform.translate);

	jsonPath = "gameobject/" + GetName();
	JsonCoordinator::Load(objectName_, jsonPath);

}

void BaseGameObject::Update(){

	model_->Update();

}

void BaseGameObject::Draw(){

	model_->Draw();

}

//===================================================================*/
//                   getter/setter
//===================================================================*/


void BaseGameObject::SetName(const std::string& name){

	objectName_ = name;
	allGameObjects_.emplace_back(this);
}

const std::vector<BaseGameObject*>& BaseGameObject::GetGameObjects(){
	return allGameObjects_;
}

//===================================================================*/
//                    imgui/ui
//===================================================================*/

void BaseGameObject::ImGui(){

	// JsonCoordinator を利用して保存・ロードボタンを表示
	if (ImGui::Button("Save")){
		JsonCoordinator::Save(objectName_, jsonPath);
	}

	// JsonCoordinator に登録したプロパティを表示
	JsonCoordinator::RenderGroupUI(objectName_);

	ImGui::Separator();
}

void BaseGameObject::ShowDebugUI(){
#ifdef _DEBUG
	if (ImGui::BeginTabItem("GameObject")){
		// ユニークなIDスコープを作成する
		for (size_t i = 0; i < BaseGameObject::GetGameObjects().size(); ++i){
			auto* gameObject = BaseGameObject::GetGameObjects()[i];

			// ImGui::PushIDを使用してユニークなスコープを設定
			ImGui::PushID(static_cast< int >(i));

			// 各オブジェクトのUIを個別に管理
			if (ImGui::CollapsingHeader(gameObject->GetName().c_str())){
				gameObject->ImGui();
			}

			// IDスコープを終了
			ImGui::PopID();
		}
		ImGui::EndTabItem();
	}
#endif // _DEBUG
}

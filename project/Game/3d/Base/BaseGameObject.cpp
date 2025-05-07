#include "BaseGameObject.h"

#include "externals/imgui/imgui.h"
#include "Engine/core/Json/JsonCoordinator.h"


BaseGameObject::BaseGameObject(const std::string& modelName) {

	auto dotPos = modelName.find_last_of('.');
	if (dotPos != std::string::npos) {
		std::string extension = modelName.substr(dotPos);

		// obj
		if (extension == ".obj") {
			objectModelType_ = ObjectModelType::ModelType_Static;
			model_ = std::make_unique<Model>(modelName);
		}
		// gltf
		else if (extension == ".gltf") {
			objectModelType_ = ObjectModelType::ModelType_Animation;
			model_ = std::make_unique<AnimationModel>(modelName);
		}
		// その他の拡張子の場合はここに追加
		else {
			// Handle other extensions or set a default type
			objectModelType_ = ObjectModelType::ModelType_Unknown;
		}

	}
}

BaseGameObject::BaseGameObject(const std::string& modelName,
							   std::optional<std::string> objectName,
							   std::function<void(IMeshRenderable*)> registerCB) {
	auto dotPos = modelName.find_last_of('.');
	if (dotPos != std::string::npos) {
		std::string extension = modelName.substr(dotPos);

		// obj
		if (extension == ".obj") {
			objectModelType_ = ObjectModelType::ModelType_Static;
			model_ = std::make_unique<Model>(modelName);
		}
		// gltf
		else if (extension == ".gltf") {
			objectModelType_ = ObjectModelType::ModelType_Animation;
			model_ = std::make_unique<AnimationModel>(modelName);
		} else {
			objectModelType_ = ObjectModelType::ModelType_Unknown;
		}

	}

	// 名前を設定
	if (objectName.has_value()) {
		SetName(objectName.value());
	} else {
		// 名前が指定されていない場合は、デフォルトの名前を設定
		const std::string defaultName = modelName + "object";
		SetName(defaultName);
	}

	//モデル登録コールバック
	registerCB(model_.get());
}

BaseGameObject::~BaseGameObject() {}


void BaseGameObject::Initialize() {}

void BaseGameObject::Update() {

	if (objectModelType_ != ObjectModelType::ModelType_Unknown) {

		model_->Update();

	}

}

void BaseGameObject::Draw() {


}


//===================================================================*/
//                   getter/setter
//===================================================================*/
void BaseGameObject::SetName(const std::string& name) {
	SceneObject::SetName(name, ObjectType::GameObject);
}



//===================================================================*/
//                    imgui/ui
//===================================================================*/
void BaseGameObject::ShowGui() {
	ImGui::Spacing();

	model_->ShowImGuiInterface();

	ImGui::Spacing();

	DerivativeGui();
}

void BaseGameObject::DerivativeGui() {
	ImGui::SeparatorText("derivative");
}

//===================================================================*/
//                    load/save
//===================================================================*/
//void BaseGameObject::SaveToJson(const std::string& fileName) const{}
//
//void BaseGameObject::LoadFromJson(const std::string& fileName){}
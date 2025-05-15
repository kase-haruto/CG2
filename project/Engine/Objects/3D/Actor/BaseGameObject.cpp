#include "BaseGameObject.h"

#include <Engine/Foundation/Json/JsonCoordinator.h>
#include <Engine/objects/Collider/BoxCollider.h>
#include <Engine/objects/Collider/SphereCollider.h>

#include "externals/imgui/imgui.h"

BaseGameObject::BaseGameObject(const std::string& modelName){

	auto dotPos = modelName.find_last_of('.');
	if (dotPos != std::string::npos){
		std::string extension = modelName.substr(dotPos);

		// obj
		if (extension == ".obj"){
			objectModelType_ = ObjectModelType::ModelType_Static;
			model_ = std::make_unique<Model>(modelName);
		}
		// gltf
		else if (extension == ".gltf"){
			objectModelType_ = ObjectModelType::ModelType_Animation;
			model_ = std::make_unique<AnimationModel>(modelName);
		}
		// その他の拡張子の場合はここに追加
		else{
			// Handle other extensions or set a default type
			objectModelType_ = ObjectModelType::ModelType_Unknown;
		}

	}
}

BaseGameObject::BaseGameObject(const std::string& modelName,
							   std::optional<std::string> objectName,
							   std::function<void(IMeshRenderable*, const WorldTransform*)> registerCB){
	auto dotPos = modelName.find_last_of('.');
	if (dotPos != std::string::npos){
		std::string extension = modelName.substr(dotPos);

		// obj
		if (extension == ".obj"){
			objectModelType_ = ObjectModelType::ModelType_Static;
			model_ = std::make_unique<Model>(modelName);
		}
		// gltf
		else if (extension == ".gltf"){
			objectModelType_ = ObjectModelType::ModelType_Animation;
			model_ = std::make_unique<AnimationModel>(modelName);
		} else{
			objectModelType_ = ObjectModelType::ModelType_Unknown;
		}

	}

	// 名前を設定
	if (objectName.has_value()){
		SetName(objectName.value());
	} else{
		// 名前が指定されていない場合は、デフォルトの名前を設定
		const std::string defaultName = modelName + "object";
		SetName(defaultName);
	}

	//モデル登録コールバック
	registerCB(model_.get(),&worldTransform_);

	//===================================================================*/
	//			collider 設定
	//===================================================================*/
	SwitchCollider(ColliderKind::Box,true); // 初期化時にBoxをセット
}

BaseGameObject::~BaseGameObject(){}


void BaseGameObject::Update(){

	if (objectModelType_ != ObjectModelType::ModelType_Unknown){

		model_->Update();

	}

	worldTransform_.Update();

	// collider の更新
	if (collider_){
		Vector3 worldPos = GetCenterPos();
		Quaternion worldRot = worldTransform_.rotation;
		collider_->Update(worldPos, worldRot);
		collider_->Draw();
	}

}


//===================================================================*/
//                    コライダー形状の変更
//===================================================================*/
void BaseGameObject::SwitchCollider(ColliderKind kind, bool isCollisionEnubled){
	if (kind == currentColliderKind_) return;

	switch (kind){
		case ColliderKind::Box:
		{
			auto box = std::make_unique<BoxCollider>(isCollisionEnubled);
			box->SetName(GetName() + "_BoxCollider");
			box->Initialize(Vector3(1.0f, 1.0f, 1.0f)); // 適当な初期サイズ
			collider_ = std::move(box);
			break;
		}
		case ColliderKind::Sphere:
		{
			auto sphere = std::make_unique<SphereCollider>(isCollisionEnubled);
			sphere->SetName(GetName() + "_SphereCollider");
			sphere->Initialize(1.0f); // 適当な初期半径
			collider_ = std::move(sphere);
			break;
		}
	}
	currentColliderKind_ = kind;
}


//===================================================================*/
//                    imgui/ui
//===================================================================*/
void BaseGameObject::ShowGui(){
	ImGui::Spacing();

	SceneObject::ShowGui();

	model_->ShowImGuiInterface();

	ImGui::Spacing();

	collider_->ShowGui();

	DerivativeGui();
}

void BaseGameObject::DerivativeGui(){
	ImGui::SeparatorText("derivative");
}


//===================================================================*/
//                   getter/setter
//===================================================================*/
void BaseGameObject::SetName(const std::string& name){
	SceneObject::SetName(name, ObjectType::GameObject);
}

void BaseGameObject::SetTranslate(const Vector3& pos){
	if (model_){
		worldTransform_.translation = pos;
	}
}

void BaseGameObject::SetScale(const Vector3& scale){
	if (model_){
		worldTransform_.scale = scale;
	}
}

const Vector3 BaseGameObject::GetCenterPos()const{
	const Vector3 offset = {0.0f, 0.5f, 0.0f};
	Vector3 worldPos = Vector3::Transform(offset, worldTransform_.matrix.world);
	return worldPos;
}

void BaseGameObject::SetColor(const Vector4& color){
	if (model_){
		model_->SetColor(color);
	}
}

void BaseGameObject::SetUvScale(const Vector3& uvScale){
	if (model_){
		model_->SetUvScale(uvScale);
	}
}

void BaseGameObject::SetCollider(std::unique_ptr<Collider> collider){
	collider_ = std::move(collider);
}

Collider* BaseGameObject::GetCollider(){ return collider_.get(); }




//===================================================================*/
//                    load/save
//===================================================================*/
//void BaseGameObject::SaveToJson(const std::string& fileName) const{}
//
//void BaseGameObject::LoadFromJson(const std::string& fileName){}
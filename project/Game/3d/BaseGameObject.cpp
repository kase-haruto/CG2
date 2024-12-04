#include "BaseGameObject.h"

#include "externals/imgui/imgui.h"

// 静的メンバ変数の定義
std::vector<BaseGameObject*> BaseGameObject::allGameObjects_;

BaseGameObject::BaseGameObject(const std::string& modelName){

	model_ = std::make_unique<Model>(modelName);
	model_->Initialize();

}


void BaseGameObject::Initialize(){


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

    ImGui::DragFloat3("scale", &model_->transform.scale.x, 0.01f);
    ImGui::DragFloat3("rotate", &model_->transform.rotate.x, 0.01f);
    ImGui::DragFloat3("translation", &model_->transform.translate.x, 0.01f);

    Vector4 color = model_->GetColor();
    ImGui::ColorEdit4("color", &color.x);
    model_->SetColor(color);

}

void BaseGameObject::ShowDebugUI(){

#ifdef _DEBUG
    if (ImGui::BeginTabItem("GameObject")){
        for (auto* gameObject : BaseGameObject::GetGameObjects()){
            if (ImGui::CollapsingHeader(gameObject->GetName().c_str())){
                gameObject->ImGui();
            }
        }
        ImGui::EndTabItem();
    }
#endif // _DEBUG

}
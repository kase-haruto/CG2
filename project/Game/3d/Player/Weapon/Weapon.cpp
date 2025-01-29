#include "Weapon.h"

#include "Engine/core/Json/JsonCoordinator.h"

#include <externals/imgui/imgui.h>
#include "Engine/core/System.h"
#include "Engine/graphics/GraphicsGroup.h"
#include "Engine/graphics/camera/CameraManager.h"
#include "Engine/core/Clock/ClockManager.h"

Weapon::Weapon(const std::string& modelName)
    : BaseGameObject(modelName){
    BaseGameObject::SetName("weapon");
}

//===================================================================*/
//                    main functions
//===================================================================*/
void Weapon::Initialize(){
    BaseGameObject::Initialize();

    std::string path = BaseGameObject::jsonPath + "/weapon";
    JsonCoordinator::LoadGroup(BaseGameObject::GetName(), path);

    model_->transform.translate = INITIAL_TRANSLATE;
    model_->transform.rotate = INITIAL_ROTATE;

    // トレイルの初期化
    swordTrail_.Initialize();
}

void Weapon::Update(){
    BaseGameObject::Update();

    // トレイルの更新
    UpdateTrail(ClockManager::GetInstance()->GetDeltaTime());
}

void Weapon::Draw(){
    // トレイルの描画
    DrawTrail();
    BaseGameObject::Draw();
}

//===================================================================*/
//                    gui/ui
//===================================================================*/
void Weapon::ShowGui(){
    ImGui::Text(BaseGameObject::GetName().c_str());

    if (ImGui::Button("SaveWeaponData")){
        std::string path = BaseGameObject::jsonPath + "/weapon";
        JsonCoordinator::SaveGroup(BaseGameObject::GetName(), path);
    }

    ImGui::Spacing();

    BaseGameObject::ShowGui();

    ImGui::Spacing();

    ImGui::DragFloat3("offset", &offset_.x, 0.01f);

    // トレイルの設定
    ImGui::Separator();
    ImGui::Text("Trail Settings");
    float fadeSpeed = swordTrail_.GetFadeSpeed();
    float minAlpha = swordTrail_.GetMinAlpha();
    if (ImGui::DragFloat("Fade Speed", &fadeSpeed, 0.1f, 0.0f, 10.0f)){
        swordTrail_.SetFadeSpeed(fadeSpeed);
    }
    if (ImGui::DragFloat("Min Alpha", &minAlpha, 0.01f, 0.0f, 1.0f)){
        swordTrail_.SetMinAlpha(minAlpha);
    }
}

void Weapon::InitializeTransform(){
    // 現在の変換値から初期値に向かって線形補間
    model_->transform.translate = Vector3::Lerp(model_->transform.translate, INITIAL_TRANSLATE, 0.2f);
    model_->transform.rotate = Vector3::Lerp(model_->transform.rotate, INITIAL_ROTATE, 0.2f);
    model_->transform.scale = Vector3::Lerp(model_->transform.scale, INITIAL_SCALE, 0.2f);
}

Vector3 Weapon::ComputeTipWorldPosition() const{
    // ローカル先端座標を取得
    Vector3 tipLocalPosition = tipPos_;
    // ワールド行列を用いてローカル座標をワールド空間に変換
    Vector3 tipWorldPosition = Vector3::Transform(tipLocalPosition, model_->worldMatrix);
    return tipWorldPosition;
}


//===================================================================*/
//                    Trail functions
//===================================================================*/
void Weapon::AddTrailSegment(const Vector3& tip, const Vector3& base){
    if (!isComboActive_) return; // コンボ中のみ追加

    // 無効な座標を追加しない
    if (tip == Vector3(0, 0, 0) || base == Vector3(0, 0, 0)) return;

    swordTrail_.AddSegment(tip, base);
}

void Weapon::ClearTrail(){
    swordTrail_.Clear();
    hasPrevFrame_ = false;
}

void Weapon::UpdateTrail(float deltaTime){
    Vector3 tip = ComputeTipWorldPosition();
    Vector3 base = GetBasePos();

    if (!hasPrevFrame_){
        prevTip_ = tip;
        prevBase_ = base;
        hasPrevFrame_ = true;
    } else{
        swordTrail_.AddSegment(tip, base);
        prevTip_ = tip;
        prevBase_ = base;
    }

    swordTrail_.Update(deltaTime);
}

void Weapon::DrawTrail(){
    swordTrail_.Draw();
}


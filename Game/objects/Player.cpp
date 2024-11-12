#include "objects/Player.h"
#include "myFunc/MyFunc.h"
#include "myFunc/MathFunc.h"
#include "core/Input.h"
#include "Collision/CollisionManager.h"
#include "lib/myFunc/PrimitiveDrawer.h"

#include "imgui.h"

#include <algorithm>
#undef min

void Player::Initialize(Model* model){
	Character::Initialize(model);

	beam_ = std::make_unique<Model>("beam");
    beam_->transform.scale = {0.5f,0.5f,50.0f};

    hpSprite_ = std::make_unique<Sprite>("white1x1.png");
    // スプライトの初期化
    hpSprite_->Initialize({120.0f,500.0f}, {64.0f,256.0f});
    hpSprite_->SetAnchorPoint({0.0f,1.0f});

    reticleSprite_ = std::make_unique<Sprite>("reticle.png");
    Vector2 mousePos = Input::GetMousePosition();
    reticleSprite_->Initialize(mousePos, {64.0f,64.0f});
    reticleSprite_->SetAnchorPoint({0.5f,0.5f});

	line_.color = {1.0f,0.0f,0.0f,1.0f};

}

void Player::Update(){
#ifdef _DEBUG
    ImGui::Begin("player");
    ImGui::DragFloat3("beamPos", &beam_->transform.translate.x, 0.01f);
    ImGui::Text("Shoot Timer: %.2f", shootTimer_);
    ImGui::Text("Fully Recovered: %s", isFullyRecovered_ ? "Yes" : "No");
    ImGui::End();
#endif // _DEBUG
    UpdateUI();

    if (!beam_->viewProjection_){
        beam_->SetViewProjection(pViewProjection_);
    }

    // 古い球の削除
    bullets_.remove_if([] (const std::unique_ptr<Bullet>& bullet){
        if (!bullet->GetIsActive()){
            CollisionManager::GetInstance()->RemoveCollider(bullet.get());
            return true;
        }
        return false;
                       });

    ReticleUpdate();
    Shoot();

    // 行列の更新
    model_->worldMatrix = MakeAffineMatrix(model_->transform.scale,
                                           model_->transform.rotate,
                                           model_->transform.translate);

    if (parentTransform_){
        parentWorldMat_ = MakeAffineMatrix({1.0f, 1.0f, 1.0f},
                                           parentTransform_->rotate,
                                           parentTransform_->translate);
        model_->worldMatrix = Matrix4x4::Multiply(parentWorldMat_, model_->worldMatrix);
    }

    model_->UpdateMatrix();
    BeamUpdate();
}

void Player::UpdateUI(){
    float maxSpriteHeight = 256.0f;
    float minSpriteHeight = 0.0f;
    float spriteHeight = std::lerp(minSpriteHeight, maxSpriteHeight, shootTimer_ / maxShootTime_);

    // スプライトのサイズを更新
    hpSprite_->SetSize({64.0f, spriteHeight});

    // 残り時間の割合を計算
    float percentage = shootTimer_ / maxShootTime_;
    Vector4 color;

    // 割合に応じて色を設定
    if (percentage <= 0.2f){
        // 0〜20%：赤色
        color = {1.0f, percentage / 0.2f, 0.0f, 1.0f}; // 赤からオレンジへの遷移
    } else if (percentage <= 0.5f){
        // 20〜50%：オレンジから黄色への遷移
        float t = (percentage - 0.2f) / 0.3f;
        color = {1.0f, std::lerp(percentage / 0.5f, 1.0f, t), 0.0f, 1.0f};
    } else{
        // 50〜100%：黄色から緑への遷移
        float t = (percentage - 0.5f) / 0.5f;
        color = {std::lerp(1.0f, 0.0f, t), 1.0f, 0.0f, 1.0f};
    }

    // スプライトの色を設定
    hpSprite_->SetColor(color);

    hpSprite_->Update();
}

void Player::Shoot(){
    // 初期化
    isShoot_ = false;

    // タイマーが完全回復していない場合は発射不可
    if (!isFullyRecovered_){
        // タイマーが完全回復したらフラグをリセット
        shootTimer_ = std::min(shootTimer_ + recoveryRate_, maxShootTime_);
        if (shootTimer_ >= maxShootTime_){
            isFullyRecovered_ = true;
        }
        return;
    }

    // スペースキーが押されていて、かつshootTimer_が0以上で発射可能
    if (Input::PushKey(DIK_SPACE) && shootTimer_ > 0.0f){
        isShoot_ = true;
        shootTimer_ -= 0.1f;  // 発射中はタイマーを減少

        // shootTimer_が0以下になったら発射不可に設定
        if (shootTimer_ <= 0.0f){
            shootTimer_ = 0.0f;
            isFullyRecovered_ = false;
        }
    } else{
        // 発射していない場合はタイマーを回復
        shootTimer_ = std::min(shootTimer_ + recoveryRate_, maxShootTime_);
        if (shootTimer_ >= maxShootTime_){
            isFullyRecovered_ = true;
        }
    }
}



void Player::BeamUpdate(){
    // ビームの衝突用座標の更新
    Vector3 offset {0.0f, -0.1f, 0.0f};
    line_.startPos = GetCenterPos() + offset;

    if (isShoot_){
        line_.endPos = reticlePos_;
    } else{
        line_.endPos = GetCenterPos() + offset;
    }

    // ビームの相対位置（プレイヤーからのオフセット位置）を設定
    Vector3 beamFixedOffset {0.0f, -0.5f, 0.0f};

    // プレイヤーのワールド行列を使ってビームのワールド位置を設定
    Vector3 worldPosition = Matrix4x4::Transform(beamFixedOffset, model_->worldMatrix);
    beam_->transform.translate = worldPosition;

    // LookAt 行列を使ってビームが reticlePos_ の方向を向くように回転
    beam_->transform.rotate = Matrix4x4::LookAtDirection(beam_->transform.translate, reticlePos_);

    // カメラの視点とプロジェクション行列を適用
    if (beam_->viewProjection_){
        beam_->worldMatrix = Matrix4x4::Multiply(beam_->viewProjection_->matView, beam_->worldMatrix);
        beam_->worldMatrix = Matrix4x4::Multiply(beam_->viewProjection_->matProjection, beam_->worldMatrix);
    }

    // ビームのローカル行列を作成して更新
    beam_->worldMatrix = MakeAffineMatrix(beam_->transform.scale,
                                          beam_->transform.rotate,
                                          beam_->transform.translate);

    beam_->UpdateMatrix();
}




void Player::ReticleUpdate(){
	// マウス座標を取得
	Vector2 mousePos = Input::GetMousePosition();

	// ビュープロジェクションビューポート合成行列
	Matrix4x4 matViewport = Matrix4x4::MakeViewportMatrix(0, 0, WinApp::kWindowWidth, WinApp::kWindowHeight, 0, 1);
	Matrix4x4 matVPV = Matrix4x4::Multiply(Matrix4x4::Multiply(pViewProjection_->matView, pViewProjection_->matProjection), matViewport);
	Matrix4x4 matInverseVPV = Matrix4x4::Inverse(matVPV);

	// スクリーン座標
	Vector3 posNear = Vector3(mousePos.x, mousePos.y, 0);
	Vector3 posFar = Vector3(mousePos.x, mousePos.y, 1);

	// スクリーン座標系からワールド座標系
	posNear = Matrix4x4::Transform(posNear, matInverseVPV);
	posFar = Matrix4x4::Transform(posFar, matInverseVPV);

	// マウスレイの方向
	Vector3 mouseDirection = posFar - posNear;
	// カメラから参照オブジェクトの距離
	const float kDistanceTestObject = 100.0f;
	reticlePos_ = posNear + (mouseDirection.Normalize() * kDistanceTestObject);

    reticleSprite_->SetPosition(mousePos);
    reticleSprite_->Update();
}



void Player::Draw(){
	if (isShoot_){
		beam_->Draw();
	}
}

void Player::DrawUi(){
    hpSprite_->Draw();
    reticleSprite_->Draw();
}

void Player::OnCollision(Collider* other){

}

const Vector3 Player::GetCenterPos() const{
	const Vector3 offset = {0.0f,0.0f,0.0f};
	Vector3 worldPos = Matrix4x4::Transform(offset, model_->worldMatrix);
	return worldPos;
}

const Vector3 Player::GetForwardVector() const{
	return Vector3 {
		model_->worldMatrix.m[2][0], // 3列目（Z軸）のX成分
		model_->worldMatrix.m[2][1], // 3列目（Z軸）のY成分
		model_->worldMatrix.m[2][2]  // 3列目（Z軸）のZ成分
	}.Normalize();
}

const Vector3 Player::GetRightVector() const{
	return Vector3 {
		model_->worldMatrix.m[0][0], // 1列目（X軸）のX成分
		model_->worldMatrix.m[0][1], // 1列目（X軸）のY成分
		model_->worldMatrix.m[0][2]  // 1列目（X軸）のZ成分
	}.Normalize();
}
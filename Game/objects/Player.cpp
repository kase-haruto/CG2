#include "objects/Player.h"
#include "myFunc/MyFunc.h"
#include "myFunc/MathFunc.h"
#include "core/Input.h"
#include "Collision/CollisionManager.h"
#include "lib/myFunc/PrimitiveDrawer.h"

#include "imgui.h"

void Player::Initialize(Model* model){
	Character::Initialize(model);

	beam_ = std::make_unique<Model>("beam");
    beam_->transform.scale = {0.5f,0.5f,50.0f};

	line_.color = {1.0f,0.0f,0.0f,1.0f};

}

void Player::Update(){
#ifdef _DEBUG
    ImGui::Begin("player");
    ImGui::DragFloat3("beamPos",&beam_->transform.translate.x,0.01f);
    ImGui::End();
#endif // _DEBUG

    if (!beam_->viewProjection_){
        beam_->SetViewProjection(pViewProjection_);
    }

    ///==============================
    //古い球の削除
    bullets_.remove_if([] (const std::unique_ptr<Bullet>& bullet){
        if (!bullet->GetIsActive()){
            CollisionManager::GetInstance()->RemoveCollider(bullet.get());
            return true;
        }
        return false;
                       });

    //===============================
    ReticleUpdate();
    Shoot();

    //行列の更新
    model_->worldMatrix = MakeAffineMatrix(model_->transform.scale,
                                           model_->transform.rotate,
                                           model_->transform.translate);

    //親がいたらそれも計算
    if (parentTransform_){
        parentWorldMat_ = MakeAffineMatrix({1.0f,1.0f,1.0f},
                                           parentTransform_->rotate,
                                           parentTransform_->translate);
        model_->worldMatrix = Matrix4x4::Multiply(parentWorldMat_, model_->worldMatrix);
    }

    model_->UpdateMatrix();

    BeamUpdate();
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
}



void Player::Draw(){
	if (isShoot_){
		PrimitiveDrawer::GetInstance()->DrawLine3d(line_.startPos, line_.endPos,line_.color);
		beam_->Draw();
	}
}

void Player::Shoot(){
	isShoot_ = false;
	// スペースキーが押されているかチェックし、クールタイムが終了した場合のみ発射
	if (Input::PushKey(DIK_SPACE)){
		isShoot_ = true;
	}
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
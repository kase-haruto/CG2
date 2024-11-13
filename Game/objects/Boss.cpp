#include "Boss.h"
#include "GameScene.h"

#include <algorithm>
#undef max

Boss::~Boss(){ }

void Boss::Initialize(Model* model,const Vector3& position){
	score_ = 1000;
	Enemy::Initialize(model,position);
    viewTime_ = 400;
	radius_ = 3.0f;
}

void Boss::Update(){
      
    if (life_ <= 0){
        isAlive_ = false;
    }

    if (isAlive_){

        Move();

    } else{
        // デプスなしに変更
        model_->IsNotDepth();

        Vector2 screenPos(1050.0f, 530.0f);

        // ビューポート行列を作成
        Matrix4x4 matViewport = Matrix4x4::MakeViewportMatrix(0, 0, WinApp::kWindowWidth, WinApp::kWindowHeight, 0, 1);

        // ビュープロジェクションビューポート合成行列を作成
        Matrix4x4 matVP = Matrix4x4::Multiply(pViewProjection_->matView, pViewProjection_->matProjection);
        Matrix4x4 matVPV = Matrix4x4::Multiply(matVP, matViewport);
        Matrix4x4 matInverseVPV = Matrix4x4::Inverse(matVPV);

        // スクリーン座標を設定
        Vector3 posNear = Vector3(screenPos.x, screenPos.y, 0);
        Vector3 posFar = Vector3(screenPos.x, screenPos.y, 1);

        // スクリーン座標系からワールド座標系への変換
        posNear = Matrix4x4::Transform(posNear, matInverseVPV);
        posFar = Matrix4x4::Transform(posFar, matInverseVPV);

        Vector3 direction = posFar - posNear;
        deadViewPos = posNear + (direction.Normalize() * fixedDistance);

        // モデル位置を設定
        model_->transform.translate = deadViewPos;

        // カメラ方向へのベクトルを計算
        Vector3 cameraPos = pViewProjection_->GetPos();
        Vector3 toCamera = cameraPos - deadViewPos;
        toCamera.Normalize();

        // x軸をカメラの方向に向けるための回転
        float rotationX = atan2(toCamera.y, sqrt(toCamera.x * toCamera.x + toCamera.z * toCamera.z));

        // y軸を回転させる
        static float rotationY = 0.0f;
        rotationY += 0.05f; // y軸回転速度

        // 回転を適用
        model_->transform.rotate = {0.0f, rotationY, 0.0};

        // 表示時間を減らし、タイムアウト時に削除
        if (--viewTime_ <= 0){
            Reset();
        }
    }

    // レインボーのカラーシフト (RGB成分の波を調整)
    static float colorShift = 0.0f;
    colorShift += 0.02f; // 色変化のスピードを調整
    if (colorShift > 3.0f) colorShift -= 3.0f; // 循環させる

    // RGB成分の計算
    float r = std::max(0.0f, 1.0f - fabs(colorShift - 0.0f));
    float g = std::max(0.0f, 1.0f - fabs(colorShift - 1.0f));
    float b = std::max(0.0f, 1.0f - fabs(colorShift - 2.0f));

    // モデルの色を設定
    model_->SetColor({r,g,b,1.0f});

    Character::Update();
}


void Boss::Move(){
	// 時間を進めるためのタイマー（角度やサイン波の基準）
	static float time = 0.0f;
	time += 0.03f;  // 動きの速度を調整

	if (isAlive_){
		// 初期位置の保持
		static float initialY = model_->transform.translate.y;

		// サイン波による上下運動
		float verticalMovement = sin(time) * 4.0f;  // 振幅0.1の上下運動

		// 円を描くような前後移動
		float radius = 0.2f;  // 円の半径
		float circularX = cos(time) * radius;
		float circularZ = sin(time) * radius;

		// 位置を更新
		model_->transform.translate.x += circularX;
		model_->transform.translate.y = initialY + verticalMovement;  // 初期位置に基づく上下運動
		model_->transform.translate.z += circularZ;

		// 進行方向の回転を設定
		Vector3 velocity(circularX, 0.0f, circularZ);  // XZ平面上の速度
		if (velocity.Length() > 0.0f){ // velocityが0でない場合のみ回転を設定
			velocity.Normalize();

			// Y軸の回転角度を計算
			float rotationY = atan2(velocity.x, velocity.z);

			// 回転をモデルに適用
			model_->transform.rotate.y = rotationY;
		}
	}
}

void Boss::Draw(){
    if (isSpawn_){
        Character::Draw();
    }
}

void Boss::Reset(){
    isSpawn_ = false;
    isAlive_ = true;
    life_ = 800;
}

void Boss::OnCollision(Collider* other){
	life_--;
}


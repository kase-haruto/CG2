#include "EnemySpawner.h"

//local
#include "EnemyManager.h"

//engine
#include "Engine/core/System.h"
#include "Engine/collision/CollisionManager.h"
#include "lib/myFunc/MyFunc.h"
#include "Engine/graphics/camera/CameraManager.h"
#include "Engine/core/Json/JsonCoordinator.h"

//c++
#include <externals/imgui/imgui.h>

EnemySpawner::EnemySpawner(const std::string& modelName)
	:BaseGameObject(modelName){
	CollisionManager::GetInstance()->AddCollider(this);
	//オブジェクトの名前を設定
	BaseGameObject::SetName("EnemySpawner");
	Collider::SetName("EnemySpawner");
}

EnemySpawner::~EnemySpawner(){
	CollisionManager::GetInstance()->RemoveCollider(this);
}

////////////////////////////////////////////////////////////////////////
//                         main function
////////////////////////////////////////////////////////////////////////
void EnemySpawner::Initialize(){

	BaseGameObject::Initialize();

	//colliderの設定
	SphereCollider::Initialize(2.0f);
	ColliderType::Type_EnemySpawner;

	spawnTimer_ = 0.0f;

}

void EnemySpawner::Initialize(const Vector3& position){
	Initialize();
	model_->transform.translate = position;
}

void EnemySpawner::Update(){
	BaseGameObject::Update();

	spawnTimer_ += System::GetDeltaTime();


	//6体よりも少ないときにスポーン
	if (enemyManager_->GetEnemyCount() < 6){
		// スポーン間隔を超えたら敵をスポーン
		if (spawnTimer_ >= spawnInterval_){
			if (enemyManager_){
				// EnemyManager を使ってスポナーの位置で敵を生成
				enemyManager_->SpawnEnemy("enemy.obj", GetCenterPos());
			}
			spawnTimer_ = 0.0f;  // タイマーをリセット
		}
	}


	//colliderの更新
	shape_.center = GetCenterPos();
}

void EnemySpawner::Draw(){
	SphereCollider::Draw();
	BaseGameObject::Draw();
}


////////////////////////////////////////////////////////////////////////
//                         ui/ImGui
////////////////////////////////////////////////////////////////////////
void EnemySpawner::ShowGui(){
	SceneObject::ShowGui();

	ImGui::Spacing(); // 少しスペースを追加

	// BaseGameObjectのGUI表示
	BaseGameObject::ShowGui();

	// BoxColliderのGUI表示
	SphereCollider::ShowGui();
}

////////////////////////////////////////////////////////////////////////
//                         collision
////////////////////////////////////////////////////////////////////////
void EnemySpawner::OnCollisionEnter([[maybe_unused]] Collider* other){}

void EnemySpawner::OnCollisionStay([[maybe_unused]] Collider* other){}

void EnemySpawner::OnCollisionExit([[maybe_unused]] Collider* other){}

#include "objects/EnemyManager.h" 
#include "Collision/CollisionManager.h"

#include <cstdlib> // rand() 関数のため
#include <ctime>   // srand() の初期化

EnemyManager::EnemyManager() { 
	enemies_.clear();
} 

EnemyManager::~EnemyManager() { 
	
}

void EnemyManager::Initialize(){}

void EnemyManager::Update(){
	enemies_.remove_if([] (const std::unique_ptr<Enemy>& enemy){
		if (enemy->GetIsRemove()){
			CollisionManager::GetInstance()->RemoveCollider(enemy.get());
			return true;
		}
		return false;
					   });


	for (auto& enemy: enemies_){
  		enemy->Update();
	}
}

void EnemyManager::Draw(){
	for (auto& enemy : enemies_){
		enemy->Draw();
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
//		リストに敵の追加
/////////////////////////////////////////////////////////////////////////////////////////
void EnemyManager::AddEnemy(const Vector3& pos){
    // プレイヤーの前方ベクトルに基づいて敵の位置を決定
    Vector3 spawnPos = pos;

    // 初回実行時に乱数のシードを設定
    static bool isRandomInitialized = false;
    if (!isRandomInitialized){
        srand(static_cast< unsigned int >(time(nullptr)));
        isRandomInitialized = true;
    }

    // モデル名をランダムで選択
    std::vector<std::string> modelNames = {"ghost", "balloon", "star"};
    std::string selectedModelName = modelNames[rand() % modelNames.size()];

    // unique_ptrでModelを作成
    auto enemyModel = std::make_unique<Model>(selectedModelName);

    // モデル名に応じた色を設定
    if (selectedModelName == "star"){
        enemyModel->SetColor({1.0f, 1.0f, 0.0f, 1.0f});  // 黄色
    } else if (selectedModelName == "ghost"){
        enemyModel->SetColor({0.8f, 0.9f, 1.0f, 1.0f});  // 薄水色
    } else if (selectedModelName == "balloon"){
        // ランダムな色を生成
        float randomR = static_cast< float >(rand()) / RAND_MAX;
        float randomG = static_cast< float >(rand()) / RAND_MAX;
        float randomB = static_cast< float >(rand()) / RAND_MAX;
        enemyModel->SetColor({randomR, randomG, randomB, 1.0f});
    }

    // Enemyオブジェクトを作成し、生ポインタを渡して初期化
    auto enemy = std::make_unique<Enemy>();
    enemy->Initialize(enemyModel.get(), spawnPos); // 生ポインタを渡す
    enemy->SetViewProjection(pViewProjection_);
    enemy->SetRadius(1.5f);

    enemies_.emplace_back(std::move(enemy));

    // enemies_がモデルの所有権を持たない場合、unique_ptrの解放を避けるためreleaseを使用
    enemyModel.release();
}



void EnemyManager::Reset(){
	enemies_.clear();
}

#pragma once

#include "ParticleSystem.h"

//* lib
#include <vector>
#include <memory>
#include <unordered_map>

struct ParticleGroup{
	std::string textureFilePath;			 // テクスチャファイルのパス
	std::unique_ptr<BaseParticle> particles; // パーティクル

	void Initialize(const std::string& texturePath);
	void Update();
	void Draw();
};

class ParticleManager{
public:
	//===================================================================*/
	//                   singleton methods
	//===================================================================*/
	static ParticleManager* GetInstance();

	// コピーコンストラクタと代入演算子を削除
	ParticleManager(const ParticleManager&) = delete;
	ParticleManager& operator=(const ParticleManager&) = delete;

public:
	//===================================================================*/
	//                   public methods
	//===================================================================*/
	// 新しいパーティクルグループを作成
	void CreateParticleGroup(const std::string& name, const std::string& textureFilePath);

	// 更新処理
	void Update();

	// 描画処理
	void Draw();

	void Finalize();

private:
	//===================================================================*/
	//                   private methods
	//===================================================================*/
	ParticleManager() = default;
	~ParticleManager() = default;

private:
	//===================================================================*/
	//                   private methods
	//===================================================================*/
	std::unordered_map<std::string, ParticleGroup> particleGroups_; // グループ名をキーにしたマップ
};
#pragma once
/* ========================================================================
/* include
/* ===================================================================== */

//engine
#include "engine/Editor/BaseEditor.h"

//c++
#include <vector>
#include <memory>

class ParticleManager;
class Particle;

class ParticleEditor :
	public BaseEditor{
public:
	//===================================================================*/
	//                    public functions
	//===================================================================*/
	ParticleEditor(ParticleManager* m);	//< コンストラクタ
	~ParticleEditor() = default;		//< デストラクタ

	//--------- main -----------------------------------------------------
	void ShowImGuiInterface()override;	//< UIの描画

	//--------- json -----------------------------------------------------
	void SaveToJson(const std::string& filePath);	//< JSONに保存
	void LoadFromJson(const std::string& filePath);	//< JSONから読み込み

private:
	//===================================================================*/
	//                    private functions
	//===================================================================*/
	void ShowParticleMakingGui();	//< パーティクルエディタの描画
	void ShowParticleList();		//< パーティクルリストの描画
	void ShowParticleProperty();	//< パーティクルプロパティの描画

	void AddParticle(std::unique_ptr<Particle> particle);	//< パーティクルの追加
	void RemoveParticle(int index);							//< パーティクルの削除
private:
	//===================================================================*/
	//                    private variables
	//===================================================================*/
	std::vector<std::string> particleList_;			//< パーティクルのリスト
	int selectedParticleIndex_ = -1;				//< 現在選択されているパーティクルのインデックス
	ParticleManager* particleContainer_ = nullptr;	//< パーティクルマネージャーのポインタ

private:
	const std::string directoryPath_ = "Resources/particles/";	//< パーティクルのディレクトリパス
};


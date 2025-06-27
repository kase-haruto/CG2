#pragma once
/* ========================================================================
/*	include space
/* ===================================================================== */

// engine
#include <Engine/Application/Effects/Particle/Detail/ParticleDetail.h>
#include <Engine/Application/Effects/Particle/FxUnit.h>
#include <Engine/Application/Effects/Particle/Module/Container/FxModuleContainer.h>
#include <Engine/Application/Effects/Particle/Parm/FxParm.h>
#include <Engine/Graphics/Buffer/DxConstantBuffer.h>
#include <Engine/Graphics/Buffer/DxStructuredBuffer.h>
#include <Engine/Graphics/Material.h>
#include <Engine/Objects/ConfigurableObject/ConfigurableObject.h>
#include <Data/Engine/Configs/Scene/Objects/Particle/EmitterConfig.h>

// c++
#include <vector>

// forward declaration
struct Vector3;

/* ========================================================================
/*	particle emitter
/* ===================================================================== */
class FxEmitter:
public ConfigurableObject<EmitterConfig>{
public:
	//===================================================================*/
	//					public func
	//===================================================================*/
	FxEmitter();
	~FxEmitter() = default;

	virtual void Update();
	void ResetFxUnit(FxUnit& fxUnit);
	void ShowGui();
	void TransferParticleDataToGPU();

	//--------- config -------------------------------------------------//
	void ApplyConfig()override;
	void ExtractConfig()override;

	//--------- accessor -------------------------------------------------//
	const std::vector<FxUnit>& GetUnits()const{ return units_; }
	const std::string& GetModelPath() const{ return modelPath; }
	const std::string& GetTexturePath() const{ return texturePath; }
	const ParticleMaterial& GetMaterial() const{ return material_; }
	const DxConstantBuffer<ParticleMaterial>& GetMaterialBuffer() const{ return materialBuffer_; }
	const DxStructuredBuffer<ParticleConstantData>& GetInstanceBuffer() const{ return instanceBuffer_; }
	bool IsDrawEnable(){ return isDrawEnable_; }
	void SetDrawEnable(bool isEnable){ isDrawEnable_ = isEnable; }
private:
	//===================================================================*/
	//					private func
	//===================================================================*/
	void Emit();
	void Emit(const Vector3& pos);

public:
	//===================================================================*/
	//					public variable
	//===================================================================*/
	Vector3 position_;					//< emitterの位置
	Vector3 prevPostion_;				//< 前回の座標
	float emitRate_ = 0.1f;				//< パーティクル生成レート
	FxParam<Vector3> velocity_;			//< パーティクルの速度（定数またはランダム）
	FxParam<float> lifetime_;			//< パーティクルの寿命（定数またはランダム）
	float defaultSize_ = 1.0f;			//< パーティクルのデフォルトサイズ

private:
	//===================================================================*/
	//					private variable
	//===================================================================*/
	std::string modelPath = "plane.obj";		//< モデルパス（デフォルトは平面
	std::string texturePath = "particle.png";	//< テクスチャパス（デフォルトはparticle.png


	const int kMaxUnits_ = 1024;			//< 最大パーティクル数
	std::vector<FxUnit> units_;				//< パーティクルユニットの配列

	std::unique_ptr<FxModuleContainer> moduleContainer_; // モジュールコンテナ

	float emitTimer_ = 0.0f;				//< パーティクル生成タイマー

	bool isFirstFrame_ = true;	//< 最初のフレームかどうか
	bool isComplement_ = true;	//< 補完を行うかどうか
	bool isStatic_ = false;		//< エミッタが静的かどうか（trueならパーティクルは動かない
	bool isDrawEnable_ = true;	//< particleを描画するか

	//resources
	ParticleMaterial material_;							//< パーティクルのマテリアル
	DxStructuredBuffer<ParticleConstantData> instanceBuffer_;
	DxConstantBuffer<ParticleMaterial> materialBuffer_; // パーティクルマテリアルの定数バッファ
};
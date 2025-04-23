#pragma once
/* ========================================================================
/* include space
/* ===================================================================== */

/* engine */
#include "engine/objects/ModelData.h"
#include "engine/graphics/Material.h"
#include "engine/objects/TransformationMatrix.h"
#include "engine/objects/Transform.h"
#include "Engine/graphics/blendMode/BlendMode.h"
#include <Engine/core/DirectX/Buffer/DxConstantBuffer.h>
#include <Engine/core/DirectX/Buffer/DxVertexBuffer.h>
#include <Engine/core/DirectX/Buffer/DxStructuredBuffer.h>

/* math */
#include "lib/myMath/Vector4.h"
#include "engine/physics/AABB.h"
#include "Engine/physics/Shape.h"
#include "lib/myFunc/Random.h"

/* c++ */
#include <list>
#include <vector>
#include <cstdint>
#include <memory>
#include <d3d12.h>
#include <wrl.h>

/* externals */
#include <externals/nlohmann/json.hpp>

enum class EmitterShape{
	OBB,
	Sphere
};

/* ========================================================================
/*	namespace
/* ===================================================================== */
namespace ParticleData{
	struct Parameters{
		EulerTransform transform {{1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f}};
		Vector3 rotationSpeed {0.0f,0.0f,0.0f};
		Vector3 velocity {};
		Vector4 color {1.0f, 1.0f, 1.0f, 1.0f};
		float lifeTime = 1.0f;
		float currentTime = 0.0f;
		Vector3 maxScale = {1.0f, 1.0f, 1.0f};

		void SetColorRandom();          // 色をランダムで初期化
		void SetColorInitialize();      // 色を初期化する(白)

		void SetVelocityRandom(float min, float max);
	};

	struct ParticleForGPU{
		Matrix4x4 wvp;
		Matrix4x4 world;
		Vector4 color;
	};

	struct EmittedPtlData{

		// 回転
		bool useRotation = true;              // 回転を使用するか
		bool rotateContinuously = false;      // 回転し続けるか（毎フレーム加算）
		bool randomizeInitialRotation = true; // 初期回転をランダムにするか
		Vector3 initialRotation = {0.0f, 0.0f, 0.0f}; // 初期回転（ランダムでなければ使用）
		Vector3 rotationSpeed = {0.0f, 0.0f, 0.0f};   // 連続回転する場合の速度（度/秒など）
	};

	struct Emitter{
		EulerTransform transform {{1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f}};    //エミッタのtransform
		uint32_t count = 1;				//発生数
		float frequency = 0.1f;			//発生頻度
		float frequencyTime = 0.1f;		//頻度用時刻
		EmitterShape shape = EmitterShape::Sphere; // エミッタの形状
		ParticleData::EmittedPtlData parmData;		//発生させるパーティクルのデータ

		void Initialize(uint32_t count);
		void Initialize(const EulerTransform& transform, const float frequency, const float frequencyTime, uint32_t count);
	};
}

/* ========================================================================
/* base class
/* ===================================================================== */
class BaseParticle{
protected:
	// カラーモード用のenumを定義
	enum class ColorMode{
		Random,
		SingleColor,
		SimilarColor
	};

	enum class BillboardAxis{
		AllAxis,
		YAxis,
		XAxis,
		ZAxis
	};

public:
	//===================================================================*/
	//                    public methods
	//===================================================================*/
	BaseParticle();
	virtual void Initialize(const std::string& modelName, const std::string& texturePath, const uint32_t count);
	virtual ~BaseParticle() = default;
	virtual void Update();
	virtual void Draw();

	virtual void ImGui();

	void VisualSettingGui();
	void ParameterGui();
	void EmitterGui();

	virtual void Emit(ParticleData::Emitter& emitter);
	void Emit();

	//--------- accessor -----------------------------------------------------
	virtual float SetParticleLifeTime() const{ return Random::Generate(0.5f, 1.0f); }
	virtual Vector3 GenerateVelocity(float speed);
	virtual bool GetUseRandomColor() const{ return true; }
	virtual Vector4 GetSelectedColor() const{ return Vector4(1.0f, 1.0f, 1.0f, 1.0f); }
	BlendMode GetBlendMode() const{ return blendMode_; }
	void SetUseRandomScale(bool useRandom){ useRandomScale_ = useRandom; }
	void SetFixedMaxScale(Vector3 scale){ fixedMaxScale_ = scale; }
	void SetRandomScaleRange(Vector3 minScale, Vector3 maxScale){
		randomScaleMin_ = minScale;
		randomScaleMax_ = maxScale;
	}
	void SetEmitterShape(EmitterShape shape){ currentShape_ = shape; }
	void SetEmitPos(const Vector3& pos);
	//--------- json -----------------------------------------------------
	virtual nlohmann::json SaveToJson() const = 0;
	virtual void LoadFromJson(const nlohmann::json& j) = 0;

protected:
	//===================================================================*/
	//                    protected methods
	//===================================================================*/

	/* resources =======================*/
	void CreateBuffer();

public:
	//===================================================================*/
	//				public methods
	//===================================================================*/
	BillboardAxis billboardAxis_ = BillboardAxis::AllAxis;
	std::vector<ParticleData::Parameters> particles_;
	bool isStatic_ = false;
	bool autoEmit_ = true;
	int32_t kMaxInstanceNum_ = 1024;
	int32_t instanceNum_ = 0;

	bool useRotation_ = false;

	bool useRandomScale_ = false;
	Vector3 fixedMaxScale_ = {1.0f, 1.0f, 1.0f};
	Vector3 randomScaleMin_ = {1.0f, 1.0f, 1.0f};
	Vector3 randomScaleMax_ = {6.0f, 6.0f, 6.0f};

	float lifeTime_ = 1.0f; // パーティクルの寿命

	bool isRandomLifeTime_ = true;
	float maxLifeTime_ = 3.0f;
	float minLifeTime_ = 1.0f;

	std::string name_;                                  // システム名
	bool useRandomColor_ = true;                        // ランダムカラーを使用するか
	Vector4 selectedColor_ = {1.0f, 1.0f, 1.0f, 1.0f};  // ランダムでない場合に使う色
	ColorMode colorMode_ = ColorMode::Random; // 現在のカラー方式
	//Vector4 selectedColor_ = {1.0f,1.0f,1.0f,1.0f}; // SINGLEまたはSIMILAR用の基準色
	float colorVariation_ = 0.1f; // 類似色モードでのバラつき度合い(0.0f〜1.0f程度)
protected:
	//===================================================================*/
	//					protected methods
	//===================================================================*/
	D3D12_GPU_DESCRIPTOR_HANDLE textureHandle; // テクスチャハンドル

	//反対側に回す回転行列
	Matrix4x4 backToFrontMatrix_;

	/* resources =======================*/
	DxConstantBuffer<Material> materialBuffer_;
	DxStructuredBuffer<ParticleData::ParticleForGPU> instancingBuffer_;

	/* data =======================*/
	std::string modelName_;                           // ▼ロードするファイル名を保持
	std::string textureName_ = "particle.png";        // ▼テクスチャのパスを保持
	std::optional<ModelData> modelData_;              // ▼取得後に代入
	Material materialData_;
	std::vector<ParticleData::ParticleForGPU> instanceDataList_;

	/* emitter ------------------------------------*/
	std::vector<ParticleData::Emitter> emitters_;

	bool emitPosX_ = true;
	bool emitNegX_ = true;
	bool emitPosY_ = true;
	bool emitNegY_ = true;
	bool emitPosZ_ = true;
	bool emitNegZ_ = true;

	bool isFixationAlpha_ = false;
	bool isBillboard_ = true;

	// 形状選択用
	EmitterShape currentShape_ = EmitterShape::Sphere;

	// blend
	BlendMode blendMode_ = BlendMode::ADD;
};

#include "SparkParticle.h"

#include "Engine/core/System.h"
#include "Engine/core/Math/Ease.h"

SparkParticle::SparkParticle(){}

void SparkParticle::Initialize(const std::string& modelName, const std::string& texturePath){
	ParticleSystem::SetName("SparkParticle");
	particleNum_ = 0;
	emitter_.Initialize(particleNum_);
	behavior_ = std::make_unique<PtlBehavior_Diffusion>();
	ParticleSystem::Initialize(modelName, texturePath);
	isStatic_ = false;
	autoEmit_ = false;
	isBillboard_ = false;
	blendMode_ = BlendMode::NORMAL;
	currentShape_ = EmitterShape::Sphere;

	// ランダムスケールを使用する
	SetUseRandomScale(true);
	SetRandomScaleRange(0.5f, 1.5f); // ランダム値の範囲を指定

	useRandomColor_ = false;
	selectedColor_ = {1.0f, 0.0f, 0.0f, 1.0f};
}

void SparkParticle::Update(){
	if (behavior_){
		behavior_->ApplyBehavior(*this);
	}

	for (auto& particle : particles_){
		// エミッターの位置を取得
		Vector3 emitterPos = emitter_.transform.translate;

		// パーティクルの位置を取得
		Vector3 particlePos = particle.transform.translate;

		// エミッターの方向を向くベクトルを計算
		Vector3 direction = (emitterPos - particlePos).Normalize();

		// デフォルトの前方ベクトル（Z軸）
		Vector3 forward = Vector3(0.0f, 0.0f, 1.0f);

		// forward → direction への回転を求める
		Quaternion rotation = Quaternion::FromEuler(forward, direction);

		// 計算した回転を適用
		particle.transform.rotate = Quaternion::ToEuler(rotation);

	}

	BaseParticle::Update();
}

Vector3 SparkParticle::GenerateVelocity(float speed){
	// 方向をランダムに生成（球状分布）
	float theta = Random::Generate(0.0f, 3.1415926535f * 2);
	float phi = Random::Generate(0.0f, 3.1415926535f);

	Vector3 velocity(
		std::sin(phi) * std::cos(theta),
		std::sin(phi) * std::sin(theta),
		std::cos(phi)
	);

	return velocity * speed;
}
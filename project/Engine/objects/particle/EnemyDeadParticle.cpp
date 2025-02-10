#include "EnemyDeadParticle.h"

EnemyDeadParticle::EnemyDeadParticle(){}

void EnemyDeadParticle::Initialize(const std::string& modelName, const std::string& texturePath){
	ParticleSystem::SetName("EnemyDeadParticle");
	particleNum_ = 0;
	emitter_.Initialize(particleNum_);
	behavior_ = std::make_unique<PtlBehavior_Tornado>();
	ParticleSystem::Initialize(modelName, texturePath);
	isStatic_ = false;
	autoEmit_ = false;
	isBillboard_ = true;

	blendMode_ = BlendMode::SUB;
	currentShape_ = EmitterShape::Sphere;

	// �����_���X�P�[�����g�p����
	SetUseRandomScale(true);
	SetRandomScaleRange(0.5f, 1.0f); // �����_���l�͈̔͂��w��

	useRandomColor_ = false;
}

void EnemyDeadParticle::Update(){
	if (behavior_){
		behavior_->ApplyBehavior(*this);
	}

	BaseParticle::Update();
}

#include "FxEmitter.h"
/* ========================================================================
/*	include space
/* ===================================================================== */
// engine
#include <Engine/Foundation/Math/Vector3.h>
#include <Engine/Application/Effects/Particle/FxUnit.h>
#include <Engine/Graphics/Context/GraphicsGroup.h>


FxEmitter::FxEmitter(){
	// マテリアルの初期化
	material_.color = Vector4(1, 1, 1, 1);
	materialBuffer_.Initialize(GraphicsGroup::GetInstance()->GetDevice());
	materialBuffer_.TransferData(material_);
}

/////////////////////////////////////////////////////////////////////////////////////////
//			更新
/////////////////////////////////////////////////////////////////////////////////////////
void FxEmitter::Update(float deltaTime){
	emitTimer_ += deltaTime;
	const float interval = 1.0f / emitRate_;
	while (emitTimer_ >= interval && unitCount_ < kMaxUnits_){
		emitTimer_ -= interval;
		Emit();
	}


	for (auto& fx : units_){
		if (!fx.alive) continue;

		fx.position += fx.velocity * deltaTime;
		fx.age += deltaTime;

		if (fx.age >= fx.lifetime)
			fx.alive = false;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
//			発生
/////////////////////////////////////////////////////////////////////////////////////////
void FxEmitter::Emit(){
	if (unitCount_ >= kMaxUnits_) return;

	FxUnit fx;
	ResetFxUnit(fx);
	units_.push_back(fx);
	unitCount_++;
}

/////////////////////////////////////////////////////////////////////////////////////////
//			リセット
/////////////////////////////////////////////////////////////////////////////////////////
void FxEmitter::ResetFxUnit(FxUnit& fx){
	fx.position = position_;
	velocity_.SetRandom(Vector3(-1.0f, 0.0f, -1.0f), Vector3(1.0f, 0.0f, 1.0f));
	lifetime_.SetRandom(1.0f, 3.0f);
	fx.velocity = velocity_.Get();
	fx.lifetime = lifetime_.Get();
	fx.age = 0.0f;
	fx.size = 1.0f;
	fx.color = Vector4(1, 1, 1, 1);
	fx.alive = true;
}
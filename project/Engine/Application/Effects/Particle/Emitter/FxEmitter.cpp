#include "FxEmitter.h"
/* ========================================================================
/*	include space
/* ===================================================================== */
// engine
#include <Engine/Foundation/Math/Vector3.h>
#include <Engine/Application/Effects/Particle/FxUnit.h>
#include <Engine/Graphics/Context/GraphicsGroup.h>
#include <Engine/Foundation/Clock/ClockManager.h>
#include <Engine/System/Command/EditorCommand/GuiCommand/ImGuiHelper/GuiCmd.h>
// externals
#include <externals/imgui/imgui.h>


FxEmitter::FxEmitter(){
	// マテリアルの初期化
	material_.color = Vector4(1, 1, 1, 1);
	materialBuffer_.Initialize(GraphicsGroup::GetInstance()->GetDevice());
	materialBuffer_.TransferData(material_);
}

/////////////////////////////////////////////////////////////////////////////////////////
//			更新
/////////////////////////////////////////////////////////////////////////////////////////
void FxEmitter::Update(){
	float deltaTime = ClockManager::GetInstance()->GetDeltaTime();
	static bool isFirstFrame = true;

	if (isFirstFrame){
		prevPostion_ = position_;
		isFirstFrame = false;

	}

	Vector3 moveDelta = position_ - prevPostion_;
	float distance = moveDelta.Length();
	if (distance > 0.0f&& isComplement_){
		float spawnInterval = 0.02f;
		int trailCount = static_cast< int >(distance / spawnInterval);
		if (trailCount > 0){
			for (int i = 0; i < trailCount; ++i){
				float dist = i * spawnInterval;
				float t = dist / distance;
				Vector3 spawnPos = Vector3::Lerp(prevPostion_, position_, t);
				Emit(spawnPos);
			}
		}
	} else{
		emitTimer_ += deltaTime;
		const float interval = emitRate_;
		if (emitTimer_ >= interval && units_.size() < kMaxUnits_){
			emitTimer_ -= interval;
			Emit();
		}

	}

	// 前回の位置を更新
	prevPostion_ = position_;

	for (auto& fx : units_){
		if (!fx.alive) continue;

		// 位置の更新
		if (!isStatic_){
			fx.position += fx.velocity * deltaTime;
		}
		// 寿命の更新
		fx.age += deltaTime;

		if (fx.age >= fx.lifetime)
			fx.alive = false;
	}

	// 死亡ユニットを削除
	std::erase_if(units_, [] (const FxUnit& fx){
		return !fx.alive;
				  });
}

/////////////////////////////////////////////////////////////////////////////////////////
//			発生
/////////////////////////////////////////////////////////////////////////////////////////
void FxEmitter::Emit(){
	Emit(position_);
}

void FxEmitter::Emit(const Vector3& pos){
	if (units_.size() >= kMaxUnits_) return;

	FxUnit fx;
	ResetFxUnit(fx);
	fx.position = pos; // ← 引数位置で初期化
	units_.push_back(fx);
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

/////////////////////////////////////////////////////////////////////////////////////////
//			gui表示
/////////////////////////////////////////////////////////////////////////////////////////
void FxEmitter::ShowGui(){
	ImGui::Begin("particle system");
	ImGui::Text("emitCount: %d", units_.size());
	GuiCmd::CheckBox("isComplement", isComplement_);
	GuiCmd::CheckBox("isStatic", isStatic_);
	ImGui::End();
}

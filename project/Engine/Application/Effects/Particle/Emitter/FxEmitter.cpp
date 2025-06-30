#include "FxEmitter.h"
/* ========================================================================
/*	include space
/* ===================================================================== */
// engine
#include <Engine/Foundation/Math/Vector3.h>
#include <Engine/Application/Effects/Particle/FxUnit.h>
#include <Engine/Graphics/Context/GraphicsGroup.h>
#include <Engine/Foundation/Clock/ClockManager.h>
#include <Engine/Application/Effects/Intermediary/FxIntermediary.h>
#include <Engine/System/Command/EditorCommand/GuiCommand/ImGuiHelper/GuiCmd.h>
#include <Data/Engine/Configs/Scene/Objects/Particle/Module/ModuleConfigFactory.h>
#include <Engine/Application/Effects/Particle/Module/Factory/ModuleFactory.h>

// externals
#include <externals/imgui/imgui.h>


FxEmitter::FxEmitter(){
	ID3D12Device* device = GraphicsGroup::GetInstance()->GetDevice().Get();
	// マテリアルの初期化
	material_.color = Vector4(1, 1, 1, 1);
	materialBuffer_.Initialize(GraphicsGroup::GetInstance()->GetDevice());

	instanceBuffer_.Initialize(device, kMaxUnits_);
	instanceBuffer_.CreateSrv(device);

	velocity_ = FxParam<Vector3>::MakeRandom(
		Vector3(-1.0f, 0.0f, -1.0f),
		Vector3(1.0f, 0.0f, 1.0f)
	);

	lifetime_ = FxParam<float>::MakeRandom(1.0f, 3.0f);
	scale_ = FxParam<Vector3>::MakeConstant();

	//モジュールの初期化
	moduleContainer_ = std::make_unique<FxModuleContainer>();
}

FxEmitter::~FxEmitter() {
	instanceBuffer_.ReleaseSrv();
}

/////////////////////////////////////////////////////////////////////////////////////////
//			更新
/////////////////////////////////////////////////////////////////////////////////////////
void FxEmitter::Update(){
	float deltaTime = ClockManager::GetInstance()->GetDeltaTime();
	if (isFirstFrame_){
		prevPostion_ = position_;
		isFirstFrame_ = false;

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

		for (auto& m : moduleContainer_->GetModules()){
			if (m->IsEnabled()){
				m->OnUpdate(fx, deltaTime);
			}
		}

		// 位置の更新
		if (!isStatic_){
			fx.position += fx.velocity * deltaTime;
		}
		// 寿命の更新
		fx.age += deltaTime;

		if (fx.age >= fx.lifetime)
			fx.alive = false;
	}

	materialBuffer_.TransferData(material_);

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

	fx.velocity = velocity_.Get();
	fx.lifetime = lifetime_.Get();
	fx.age = 0.0f;
	fx.scale = scale_.Get();
	fx.initialScale = fx.scale; // 初期スケールを設定
	fx.color = Vector4(1, 1, 1, 1);
	fx.alive = true;
}

/////////////////////////////////////////////////////////////////////////////////////////
//			gui表示
/////////////////////////////////////////////////////////////////////////////////////////
void FxEmitter::ShowGui() {
	ImGui::PushID(this);
	ConfigurableObject::ShowGUi();

	ImGui::Text("emitCount: %d", units_.size());
	GuiCmd::DragFloat3("position", position_);
	GuiCmd::DragFloat("emitRate", emitRate_, 0.01f, 0.0f, 10.0f);

	GuiCmd::CheckBox("isComplement", isComplement_);
	GuiCmd::CheckBox("isStatic", isStatic_);

	ImGuiHelpers::DrawFxParamGui("Scale", scale_);
	ImGuiHelpers::DrawFxParamGui("Velocity", velocity_);
	ImGuiHelpers::DrawFxParamGui("Lifetime", lifetime_);

	ImGui::Spacing();
	ImGui::SeparatorText("Modules");

	//=============================
	// 追加済みモジュールの編集GUI
	//=============================
	for (auto& m : moduleContainer_->GetModules()) {
		ImGui::PushID(m.get());

		bool enabled = m->IsEnabled();
		if (ImGui::Checkbox("##enabled", &enabled)) {
			m->SetEnabled(enabled);
			UpdateConfigModulesFromContainer();
		}
		ImGui::SameLine();

		bool open = ImGui::CollapsingHeader(m->GetName().c_str());
		if (open && enabled) {
			ImGui::Indent();
			m->ShowGuiContent();
			ImGui::Unindent();
		}

		ImGui::SameLine();
		if (ImGui::Button("Remove")) {
			RemoveModule(m->GetName());
			ImGui::PopID();
			break; // remove後なので break
		}
		ImGui::PopID();
	}

	//=============================
	// 追加されていないモジュールの表示
	//=============================
	ImGui::Spacing();
	ImGui::SeparatorText("Add Modules");

	static const std::vector<std::string> availableModules = {
		"GravityModule",
		"SizeOverLifetimeModule",
	};

	for (const auto& modName : availableModules) {
		if (!moduleContainer_->HasModule(modName)) {
			if (ImGui::Button(modName.c_str())) {
				AddModule(modName);
			}
		}
	}
	ImGui::NewLine();

	ImGui::PopID();
}


void FxEmitter::TransferParticleDataToGPU(){
	std::vector<ParticleConstantData> gpuUnits;
	gpuUnits.clear();
	for (const auto& fx : units_){
		if (fx.alive){
			gpuUnits.push_back({fx.position, fx.scale, fx.color});
		}
	}
	instanceBuffer_.TransferVectorData(gpuUnits);
}

/////////////////////////////////////////////////////////////////////////////////////////
//			コンフィグの適用
/////////////////////////////////////////////////////////////////////////////////////////
void FxEmitter::ApplyConfig() {
	position_ = config_.position;
	material_.color = config_.color;
	velocity_.FromConfig(config_.velocity);
	lifetime_.FromConfig(config_.lifetime);
	emitRate_ = config_.emitRate;
	modelPath = config_.modelPath;
	texturePath = config_.texturePath;
	isDrawEnable_ = config_.isDrawEnable;
	isComplement_ = config_.isComplement;
	isStatic_ = config_.isStatic;

	moduleContainer_ = std::make_unique<FxModuleContainer>(config_.modules);
}

void FxEmitter::ExtractConfig() {
	config_.position = position_;
	config_.color = material_.color;
	config_.velocity = FxVector3ParamConfig{ velocity_.ToConfig() };
	config_.lifetime = FxFloatParamConfig{ lifetime_.ToConfig() };
	config_.emitRate = emitRate_;
	config_.modelPath = modelPath;
	config_.texturePath = texturePath;
	config_.isDrawEnable = isDrawEnable_;
	config_.isComplement = isComplement_;
	config_.isStatic = isStatic_;

	config_.modules.clear();
	if (!moduleContainer_) return;

	for (const auto& mod : moduleContainer_->GetModules()) {
		auto cfgPtr = FxModuleFactory::CreateConfigFromModule(*mod);
		if (cfgPtr) {
			config_.modules.push_back(std::move(cfgPtr));
		}
	}
}

void FxEmitter::UpdateConfigModulesFromContainer() {
	config_.modules.clear();
	if (!moduleContainer_) return;

	for (const auto& m : moduleContainer_->GetModules()) {
		auto cfg = std::make_unique<SimpleModuleConfig>(m->GetName(), m->IsEnabled());
		config_.modules.push_back(std::move(cfg));
	}
}

void FxEmitter::AddModule(const std::string& moduleName) {
	if (!moduleContainer_) moduleContainer_ = std::make_unique<FxModuleContainer>();
	if (!moduleContainer_->HasModule(moduleName)) {
		moduleContainer_->AddModuleByName(moduleName);
		UpdateConfigModulesFromContainer();
	}
}

void FxEmitter::RemoveModule(const std::string& moduleName) {
	if (!moduleContainer_) return;
	moduleContainer_->RemoveModuleByName(moduleName);
	UpdateConfigModulesFromContainer();
}

void FxEmitter::SetModuleEnabled(const std::string& moduleName, bool enabled) {
	if (!moduleContainer_) return;
	for (auto& m : moduleContainer_->GetModules()) {
		if (m->GetName() == moduleName) {
			m->SetEnabled(enabled);
			break;
		}
	}
	UpdateConfigModulesFromContainer();
}
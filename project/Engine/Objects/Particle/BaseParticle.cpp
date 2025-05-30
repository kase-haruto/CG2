#include "BaseParticle.h"
/* ========================================================================
/* include space
/* ===================================================================== */
//* engine
#include <Engine/Assets/Model/ModelManager.h>
#include <Engine/Assets/Texture/TextureManager.h>
#include <Engine/Application/System/System.h>
#include <Engine/Foundation/Clock/ClockManager.h>
#include <Engine/Graphics/Camera/Manager/CameraManager.h>
#include <Engine/Graphics/Context/GraphicsGroup.h>
#include <Engine/Graphics/Descriptor/SrvLocator.h>
#include <Engine/objects/particle/ParticleEmitShape.h>
#include <Engine/Objects/LightObject/DirectionalLight.h>
#include <Engine/Renderer/Primitive/PrimitiveDrawer.h>
#include <Engine/System/Command/EditorCommand/GuiCommand/ImGuiHelper/GuiCmd.h>

//* lib
#include <Engine/Foundation/Utility/Func/MathFunc.h>
#include <Engine/Foundation/Utility/Func/MyFunc.h>
#include <Engine/Foundation/Utility/Random/Random.h>

#include <externals/imgui/imgui.h>

BaseParticle::BaseParticle(){
	particles_.clear();
}

BaseParticle::BaseParticle(const BaseParticle& other)
	: billboardAxis_(other.billboardAxis_),
	particles_(other.particles_),
	isStatic_(other.isStatic_),
	kMaxInstanceNum_(other.kMaxInstanceNum_),
	instanceNum_(other.instanceNum_),
	useRotation_(other.useRotation_),
	useRandomScale_(other.useRandomScale_),
	fixedMaxScale_(other.fixedMaxScale_),
	randomScaleMin_(other.randomScaleMin_),
	randomScaleMax_(other.randomScaleMax_),
	lifeTime_(other.lifeTime_),
	isRandomLifeTime_(other.isRandomLifeTime_),
	maxLifeTime_(other.maxLifeTime_),
	minLifeTime_(other.minLifeTime_),
	flyToEmitter_(other.flyToEmitter_),
	name_(other.name_),
	useRandomColor_(other.useRandomColor_),
	selectedColor_(other.selectedColor_),
	colorMode_(other.colorMode_),
	colorVariation_(other.colorVariation_),
	modelName_(other.modelName_),
	textureName_(other.textureName_),
	emitters_(other.emitters_),
	emitType_(other.emitType_),
	emitPosX_(other.emitPosX_),
	emitNegX_(other.emitNegX_),
	emitPosY_(other.emitPosY_),
	emitNegY_(other.emitNegY_),
	emitPosZ_(other.emitPosZ_),
	emitNegZ_(other.emitNegZ_),
	isFixationAlpha_(other.isFixationAlpha_),
	isBillboard_(other.isBillboard_),
	currentShape_(other.currentShape_),
	blendMode_(other.blendMode_)
	{textureHandle.ptr = 0; // 初期化
}

void BaseParticle::Initialize(const std::string& modelName, const std::string& texturePath,[[maybe_unused]] const uint32_t count){
	// 初期エミッターを1つ作って即時Emit（任意）
	//emitters_.clear();

	//ParticleData::Emitter defaultEmitter;
	//defaultEmitter.Initialize(5);
	//emitters_.emplace_back(defaultEmitter);

	modelName_ = modelName;
	textureName_ = texturePath;
	textureHandle.ptr = 0; // 初期化
	textureHandle = TextureManager::GetInstance()->LoadTexture(texturePath);

	backToFrontMatrix_ = MakeRotateYMatrix(std::numbers::pi_v<float>);
	instanceDataList_.reserve(kMaxInstanceNum_);

	CreateBuffer();
}

void BaseParticle::Update(){

	const float deltaTime = ClockManager::GetInstance()->GetDeltaTime();

	if (!modelData_){
		if (ModelManager::GetInstance()->IsModelLoaded(modelName_)){
			modelData_ = ModelManager::GetInstance()->GetModelData(modelName_);
			auto device = GraphicsGroup::GetInstance()->GetDevice();
			modelData_->vertexBuffer.Initialize(device, UINT(modelData_->vertices.size()));
			modelData_->vertexBuffer.TransferVectorData(modelData_->vertices);
		}
		return;
	}

	// EmitType に応じた発生処理（Auto, Both）
	if (emitType_ == EmitType::Auto || emitType_ == EmitType::Both){
		for (auto& emitter : emitters_) {
			// 最初のフレームだけ prevPosition を現在位置に初期化
			if (emitter.isFirstFrame) {
				emitter.prevPosition = emitter.transform.translate;
				emitter.isFirstFrame = false;
			}

			// EmitTypeに応じた自動発生
			if (emitter.frequency > 0.0f) {
				float emissionRate = 1.0f / emitter.frequency;
				emitter.emissionCounter += deltaTime * emissionRate;

				int emitCount = static_cast<int>(emitter.emissionCounter);
				emitter.emissionCounter -= emitCount;

				for (int i = 0; i < emitCount; ++i) {
					Emit(emitter);
				}
			}

			// 移動補間によるTrail発生
			Vector3 moveDelta = emitter.transform.translate - emitter.prevPosition;
			float distance = moveDelta.Length();
			if (distance > 0.0f) {
				float spawnInterval = 0.02f;
				int trailCount = static_cast<int>(distance / spawnInterval);
				if (trailCount > 0) {
					for (int i = 0; i < trailCount; ++i) {
						float dist = i * spawnInterval;
						float t = dist / distance;
						Vector3 spawnPos = Vector3::Lerp(emitter.prevPosition, emitter.transform.translate, t);
						Emit(emitter, spawnPos);
					}
				}
			}

			// 次フレーム用に移動後の位置を更新
			emitter.prevPosition = emitter.transform.translate;
		}

	}

	if (instanceNum_ <= 0) return;// パーティクルがない場合は何もしない
	// パーティクル更新
	instanceDataList_.clear();
	instanceNum_ = 0;

	for (auto it = particles_.begin(); it != particles_.end();){
		if (instanceNum_ >= kMaxInstanceNum_) break;

		if (it->lifeTime <= it->currentTime){
			it = particles_.erase(it);
			continue;
		}

		//回転させる
		if (it->rotationSpeed.x > 0.0f ||
			it->rotationSpeed.y > 0.0f ||
			it->rotationSpeed.z > 0.0f){
			it->transform.rotate += it->rotationSpeed * deltaTime;
		}

		// 行列計算
		Matrix4x4 worldMatrix;
		if (isBillboard_){
			// ビルボード用ワールド行列の構築
			Matrix4x4 billboard = CameraManager::GetWorldMatrix();
			billboard.m[3][0] = billboard.m[3][1] = billboard.m[3][2] = 0.0f;

			switch (billboardAxis_){
				case BillboardAxis::AllAxis:
					billboard = Matrix4x4::Multiply(backToFrontMatrix_, billboard);
					break;
				case BillboardAxis::YAxis:
					billboard = MakeYAxisBillboard(billboard);
					break;
				case BillboardAxis::XAxis:
					billboard = MakeXAxisBillboard(billboard);
					break;
				case BillboardAxis::ZAxis:
					billboard = MakeZAxisBillboard(billboard);
					break;
			}

			if (useRotation_){
				worldMatrix = Matrix4x4::Multiply(
					Matrix4x4::Multiply(MakeScaleMatrix(it->transform.scale),
					Matrix4x4::Multiply(EulerToMatrix(it->transform.rotate), billboard)),
					MakeTranslateMatrix(it->transform.translate));
			} else{
				worldMatrix = Matrix4x4::Multiply(
					Matrix4x4::Multiply(MakeScaleMatrix(it->transform.scale), billboard),
					MakeTranslateMatrix(it->transform.translate));
			}
		} else{
			// 通常の回転付き行列
			worldMatrix = Matrix4x4::Multiply(
				Matrix4x4::Multiply(MakeScaleMatrix(it->transform.scale),
				useRotation_ ? EulerToMatrix(it->transform.rotate) : Matrix4x4::MakeIdentity()),
				MakeTranslateMatrix(it->transform.translate));
		}


		// GPU転送用インスタンスデータ作成
		ParticleData::ParticleForGPU instance;
		instance.world = worldMatrix;
		instance.color = it->color;

		// アルファ設定
		if (isFixationAlpha_){
			instance.color.w = 1.0f;
		} else{
			float ratio = std::clamp(it->currentTime / it->lifeTime, 0.0f, 1.0f);
			instance.color.w = 1.0f - ratio;
		}

		instanceDataList_.push_back(instance);

		// パーティクル更新
		it->currentTime += deltaTime;
		if (!isStatic_){
			it->transform.translate += it->velocity * deltaTime;
		}

		++instanceNum_;
		++it;
	}


	if (instanceNum_ > 0){
		instancingBuffer_.TransferVectorData(instanceDataList_);
	}
	modelData_->vertexBuffer.TransferVectorData(modelData_->vertices);
	materialBuffer_.TransferData(materialData_);


}

void BaseParticle::Draw(ID3D12GraphicsCommandList* cmdList){
	CameraManager::SetCommand(cmdList, PipelineType::StructuredObject);

	if (!modelData_) return;
	if (instanceNum_ == 0) return;

	modelData_->vertexBuffer.SetCommand(cmdList);

	materialBuffer_.SetCommand(cmdList, 0);
	//t0
	cmdList->SetGraphicsRootDescriptorTable(2, instancingBuffer_.GetGpuHandle());
	//t1
	cmdList->SetGraphicsRootDescriptorTable(3, textureHandle);

	// 描画コマンド（インスタンシング）
	cmdList->DrawInstanced(
		static_cast< UINT >(modelData_->vertices.size()), // 頂点数
		instanceNum_,                                   // インスタンス数
		0, 0
	);

#ifdef _DEBUG
	// デバッグ用：エミッタの形状を描画
	/*if (currentShape_ == EmitterShape::OBB){
		PrimitiveDrawer::GetInstance()->DrawOBB(
			emitter_.transform.translate,
			emitter_.transform.rotate,
			emitter_.transform.scale,
			{1.0f, 1.0f, 1.0f, 1.0f}
		);
	} else if (currentShape_ == EmitterShape::Sphere){
		PrimitiveDrawer::GetInstance()->DrawSphere(
			emitter_.transform.translate,
			emitter_.transform.scale.x * 0.5f,
			8,
			{1.0f, 1.0f, 1.0f, 1.0f}
		);
	}*/
#endif
}


void BaseParticle::ImGui(){

	if (ImGui::CollapsingHeader("emitterType")){

		// 形状選択UI
		int shapeIndex = ( int ) currentShape_;
		const char* shapeNames[] = {"OBB", "Sphere"};
		if (ImGui::Combo("Emitter Shape", &shapeIndex, shapeNames, IM_ARRAYSIZE(shapeNames))){
			currentShape_ = ( EmitterShape ) shapeIndex;
		}

		//if (currentShape_ == EmitterShape::OBB){
		//	ImGui::DragFloat3("size", &emitter_.transform.scale.x, 0.01f);
		//	ImGui::Checkbox("Emit +X Face", &emitPosX_);
		//	ImGui::Checkbox("Emit -X Face", &emitNegX_);
		//	ImGui::Checkbox("Emit +Y Face", &emitPosY_);
		//	ImGui::Checkbox("Emit -Y Face", &emitNegY_);
		//	ImGui::Checkbox("Emit +Z Face", &emitPosZ_);
		//	ImGui::Checkbox("Emit -Z Face", &emitNegZ_);
		//}

		//if (currentShape_ == EmitterShape::Sphere){
		//	ImGui::DragFloat("radius", &emitter_.transform.scale.x, 0.01f);
		//}
	}
}

void BaseParticle::VisualSettingGui(){

	/* model/texture settings =======================*/
#pragma region model/texture settings
	//テクスチャの変更
	// ===== 表示：現在のモデルとテクスチャ ===== //
	ImGui::Text("Model Name:");
	ImGui::SameLine();
	ImGui::Text(modelName_.c_str());

	ImGui::Text("Texture Name:");
	ImGui::SameLine();
	ImGui::Text(textureName_.c_str());

	// ===== テクスチャ選択 UI ===== //
	ImGui::SeparatorText("Choose Texture");
	auto& textures = TextureManager::GetInstance()->GetLoadedTextures();
	if (ImGui::BeginCombo("Texture", textureName_.c_str())){
		for (const auto& texture : textures){
			bool is_selected = (textureName_ == texture.first);
			if (ImGui::Selectable(texture.first.c_str(), is_selected)){
				textureName_ = texture.first;
				textureHandle = TextureManager::GetInstance()->LoadTexture(textureName_);

			}
			if (is_selected){
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}

	// ===== モデル選択 UI ===== //
	ImGui::SeparatorText("Choose Model");
	const auto& models = ModelManager::GetInstance()->GetLoadedModelNames();
	if (ImGui::BeginCombo("Model", modelName_.c_str())){
		for (const auto& model : models){
			bool is_selected = (modelName_ == model);
			if (ImGui::Selectable(model.c_str(), is_selected)){
				modelName_ = model;
				modelData_ = ModelManager::GetInstance()->GetModelData(modelName_);
				if (modelData_.has_value()){
					// モデルデータがある場合、テクスチャを更新
					const auto& device = GraphicsGroup::GetInstance()->GetDevice();
					modelData_->vertexBuffer.Initialize(device, UINT(modelData_->vertices.size()));
					modelData_->indexBuffer.Initialize(device, UINT(modelData_->indices.size()));
				}
			}
			if (is_selected){
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}

	// ===== ブレンドモード ===== //
	//ブレンドモードの選択
	const char* blendModeNames[] = {
		"NONE",
		"ALPHA",
		"ADD",
		"SUB",
		"MUL",
		"NORMAL",
		"SCREEN"
	};
	int currentBlendMode = static_cast< int >(blendMode_);
	if (ImGui::Combo("Blend Mode", &currentBlendMode, blendModeNames, IM_ARRAYSIZE(blendModeNames))){
		blendMode_ = static_cast< BlendMode >(currentBlendMode);
	}

#pragma endregion

	/* color settings =======================*/
#pragma region color settings
	//ImGui::Checkbox("fixationAlpha" ,&isFixationAlpha_);
	const char* modes[] = {"Random", "Single Color", "Similar Color"};
	int currentMode = static_cast< int >(colorMode_);
	if (ImGui::Combo("Color Mode", &currentMode, modes, IM_ARRAYSIZE(modes))){
		colorMode_ = static_cast< ColorMode >(currentMode);
	}

	if (colorMode_ == ColorMode::SingleColor){
		ImGui::ColorEdit4("Particle Color", &selectedColor_.x, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
		ImGui::SameLine();
		ImGui::Text("Particle Color");
	} else if (colorMode_ == ColorMode::SimilarColor){
		ImGui::ColorEdit4("Base Color", &selectedColor_.x, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
		ImGui::SameLine();
		ImGui::Text("Base Color");

		ImGui::DragFloat("Color Variation", &colorVariation_, 0.01f, 0.0f, 1.0f);

	}
#pragma endregion
}

void BaseParticle::ParameterGui(){
	ImGui::Checkbox("isStatic", &isStatic_);
	ImGui::Checkbox("isBillboard", &isBillboard_);
	ImGui::Checkbox("useRotation", &useRotation_);

	if (isBillboard_){
		const char* axisNames[] = {"All-Axis", "Y-Axis", "X-Axis", "Z-Axis"};
		int axis = static_cast< int >(billboardAxis_);
		if (ImGui::Combo("Billboard Axis", &axis, axisNames, IM_ARRAYSIZE(axisNames))){
			billboardAxis_ = static_cast< BillboardAxis >(axis);
		}
	}

	ImGui::SeparatorText("Particle Size");
	ImGui::Checkbox("Use Random Scale", &useRandomScale_);
	if (useRandomScale_){
		ImGui::DragFloat3("Min Scale", &randomScaleMin_.x, 0.01f);
		ImGui::DragFloat3("Max Scale", &randomScaleMax_.x, 0.01f);
		if (randomScaleMin_.x > randomScaleMax_.x) std::swap(randomScaleMin_.x, randomScaleMax_.x);
		if (randomScaleMin_.y > randomScaleMax_.y) std::swap(randomScaleMin_.y, randomScaleMax_.y);
		if (randomScaleMin_.z > randomScaleMax_.z) std::swap(randomScaleMin_.z, randomScaleMax_.z);
	} else{
		ImGui::DragFloat3("scale", &fixedMaxScale_.x, 0.01f, 0.0f, 10.0f);
	}

	ImGui::SeparatorText("lifeTime");
	ImGui::Checkbox("Random LifeTime", &isRandomLifeTime_);
	if (isRandomLifeTime_){
		ImGui::DragFloat("Max", &maxLifeTime_, 0.01f, 0.0f, 10.0f);
		ImGui::DragFloat("Min", &minLifeTime_, 0.01f, 0.0f, 10.0f);
		if (minLifeTime_ > maxLifeTime_) std::swap(minLifeTime_, maxLifeTime_);
		lifeTime_ = Random::Generate<float>(minLifeTime_, maxLifeTime_);
	} else{
		ImGui::DragFloat("lifeTime", &lifeTime_, 0.01f, 0.0f, 10.0f);
	}
}


void BaseParticle::EmitterGui(){

	ImGui::Text("instanceNum: %d", instanceNum_);

	if (ImGui::CollapsingHeader("Emitters", ImGuiTreeNodeFlags_DefaultOpen)){
		int emitterIndex = 0;
		for (auto& emitter : emitters_){
			GuiCmd::DragFloat3("Emitter Position", emitter.transform.translate);

			ImGui::PushID(emitterIndex);
			std::string label = "Emitter " + std::to_string(emitterIndex);
			if (ImGui::TreeNode(label.c_str())){
				const char* emitTypeNames[] = {"Once", "Auto", "Both"};
				int emitTypeIndex = static_cast< int >(emitType_);
				if (ImGui::Combo("Emit Type", &emitTypeIndex, emitTypeNames, IM_ARRAYSIZE(emitTypeNames))){
					emitType_ = static_cast< EmitType >(emitTypeIndex);
				}

				// Emit ボタン（EmitType::Auto 以外のときのみ表示）
				if (emitType_ == EmitType::Once || emitType_ == EmitType::Both){
					if (ImGui::Button("Emit Now")){
						EmitAll(); // 全エミッタに対してEmit
					}
				}

				int shapeIndex = static_cast< int >(emitter.shape);
				const char* shapeNames[] = {"OBB", "Sphere"};
				if (ImGui::Combo("Shape", &shapeIndex, shapeNames, IM_ARRAYSIZE(shapeNames))){
					emitter.shape = static_cast< EmitterShape >(shapeIndex);
				}
				ImGui::DragFloat3("Position", &emitter.transform.translate.x, 0.01f);
				ImGui::DragFloat3("Scale", &emitter.transform.scale.x, 0.01f);
				ImGui::DragFloat("Frequency", &emitter.frequency, 0.01f, 0.0f);
				ImGui::DragInt("Count", reinterpret_cast< int* >(&emitter.count), 1, 1, 100);

				ImGui::SeparatorText("emitedParticleParm");
				ImGui::Checkbox("Use Rotation", &emitter.parmData.useRotation);
				ImGui::Checkbox("Rotate Continuously", &emitter.parmData.rotateContinuously);
				ImGui::Checkbox("Random Initial Rotation", &emitter.parmData.randomizeInitialRotation);
				ImGui::Checkbox("flyToEmitter", &flyToEmitter_);
				if (!emitter.parmData.randomizeInitialRotation){
					ImGui::DragFloat3("Initial Rotation (Euler)", &emitter.parmData.initialRotation.x, 0.1f);
				}

				if (emitter.parmData.rotateContinuously){
					ImGui::DragFloat3("Rotation Speed", &emitter.parmData.rotationSpeed.x, 0.1f);
				}


				ImGui::TreePop();
			}
			ImGui::PopID();
			emitterIndex++;
		}
		if (ImGui::Button("Add Emitter")){
			ParticleData::Emitter newEmitter;
			newEmitter.Initialize(10);
			emitters_.push_back(newEmitter);
		}
	}
}


void BaseParticle::Emit(ParticleData::Emitter& emitter, std::optional<Vector3> position){
	for (uint32_t i = 0; i < emitter.count && particles_.size() < static_cast< size_t >(kMaxInstanceNum_); ++i){
		ParticleData::Parameters particle;

		// カラー設定
		if (GetUseRandomColor()){
			particle.SetColorRandom();
		} else{
			particle.color = GetSelectedColor();
		}

		// ここで position がある場合はそれを使う
		Vector3 emitPos = position.has_value() ? position.value() : emitter.transform.translate;

		// ① 位置
		particle.transform.translate = emitPos;

		// ② 速度
		if (flyToEmitter_){
			Vector3 offset = Random::GenerateVector3(-5.0f, 5.0f);
			Vector3 spawnPos = emitPos + offset;

			particle.transform.translate = spawnPos;

			Vector3 toEmitter = emitter.transform.translate - spawnPos;
			toEmitter.Normalize();
			float speed = Random::Generate(0.5f, 2.0f);
			particle.velocity = toEmitter * speed;

			// 回転
			Vector3 forward = toEmitter;
			Vector3 up = Vector3(0.0f, 1.0f, 0.0f);
			Vector3 right = Vector3::Cross(up, forward);
			up = Vector3::Cross(forward, right);
			Matrix4x4 rotationMatrix = Matrix4x4::MakeLookRotationMatrix(forward, up);
			particle.transform.rotate = Matrix4x4::ToEuler(rotationMatrix);
		} else{
			float speed = Random::Generate(0.5f, 2.0f);
			particle.velocity = GenerateVelocity(speed);
			if (!isBillboard_){
				Matrix4x4 cam = CameraManager::GetWorldMatrix();
				particle.transform.rotate = Matrix4x4::ToEuler(cam);
			}
		}

		// 寿命やスケール、回転
		particle.lifeTime = lifeTime_;
		particle.currentTime = 0.0f;
		particle.transform.scale = useRandomScale_
			? Vector3(
			Random::Generate(randomScaleMin_.x, randomScaleMax_.x),
			Random::Generate(randomScaleMin_.y, randomScaleMax_.y),
			Random::Generate(randomScaleMin_.z, randomScaleMax_.z))
			: fixedMaxScale_;
		particle.maxScale = particle.transform.scale;

		if (emitter.parmData.useRotation){
			if (emitter.parmData.randomizeInitialRotation){
				particle.transform.rotate = {
					Random::Generate(0.0f, 360.0f),
					Random::Generate(0.0f, 360.0f),
					Random::Generate(0.0f, 360.0f)
				};
			} else{
				particle.transform.rotate = emitter.parmData.initialRotation;
			}
			if (emitter.parmData.rotateContinuously){
				particle.rotationSpeed = emitter.parmData.rotationSpeed;
			}
		} else{
			particle.transform.rotate = {0.0f, 0.0f, 0.0f};
		}

		particles_.push_back(particle);
	}
	instanceNum_ = static_cast< int32_t >(particles_.size());
}



void BaseParticle::EmitAll(){
	if (emitters_.empty()) {
		ParticleData::Emitter defaultEmitter;
		defaultEmitter.Initialize(10);
		emitters_.push_back(defaultEmitter);
	}

	for (auto& emitter:emitters_){
		Emit(emitter);
	}
}

void BaseParticle::Play(EmitType emitType){
	emitType_ = emitType;
	if (emitType_ == EmitType::Once || emitType_ == EmitType::Both){
		EmitAll();
	}
}


Vector3 BaseParticle::GenerateVelocity(float speed){
	// デフォルトのランダムな方向の速度生成
	Vector3 velocity = Random::GenerateVector3(-1.0f, 1.0f);
	return velocity * speed;
}

void BaseParticle::SetEmitPos(const Vector3& pos){
	for (auto& emitter:emitters_){
		emitter.transform.translate = pos;
	}
}

void BaseParticle::CreateBuffer(){
	ComPtr<ID3D12Device> device = GraphicsGroup::GetInstance()->GetDevice();
	assert(device && "Failed to get D3D12Device.");

	//=== マテリアル定数バッファ初期化 ===//
	materialData_.color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	materialData_.shininess = 20.0f;
	materialData_.enableLighting = HalfLambert;
	materialData_.uvTransform = Matrix4x4::MakeIdentity();

	materialBuffer_.Initialize(device);
	materialBuffer_.TransferData(materialData_);

	//=== インスタンシング用 StructuredBuffer 初期化 ===//
	if (kMaxInstanceNum_ == 0){
		throw std::runtime_error("kMaxInstanceNum_ must be greater than 0.");
	}
	instancingBuffer_.Initialize(device, kMaxInstanceNum_);
	instancingBuffer_.CreateSrv(device);
}

void ParticleData::Parameters::SetColorRandom(){

	color = {Random::Generate(0.0f, 1.0f),Random::Generate(0.0f, 1.0f),Random::Generate(0.0f, 1.0f),1.0f};

}

void ParticleData::Parameters::SetColorInitialize(){
	color = Vector4 {1.0f,1.0f,1.0f,1.0f};
}

void ParticleData::Parameters::SetVelocityRandom(float min, float max){

	velocity = {Random::Generate(min,max),Random::Generate(min,max),Random::Generate(min,max)};

}

void ParticleData::Emitter::Initialize(uint32_t Count){

	count = Count;
	transform.translate = {0.0f,0.0f,0.0f};
	transform.rotate = {0.0f,0.0f,0.0f};
	transform.scale = {1.0f,1.0f,1.0f};
	frequency = 0.5f;
	frequencyTime = 0.0f;

}

void ParticleData::Emitter::Initialize(const EulerTransform& Transform, const float Frequency, const float FrequencyTime, uint32_t Count){

	count = Count;
	transform = Transform;
	frequency = Frequency;
	frequencyTime = FrequencyTime;

}

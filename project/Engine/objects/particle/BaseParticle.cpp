#include "BaseParticle.h"

//* engine
#include "Engine/core/System.h"
#include "Engine/graphics/GraphicsGroup.h"
#include "Engine/physics/light/DirectionalLight.h"
#include "Engine/graphics/SrvLocator.h"
#include "Engine/objects/ModelManager.h"
#include "Engine/objects/TextureManager.h"
#include "Engine/graphics/camera/CameraManager.h"
#include "ParticleEmitShape.h"
#include "Engine/core/Clock/ClockManager.h"

//* lib
#include "lib/myFunc/MyFunc.h"
#include "lib/myFunc/PrimitiveDrawer.h"
#include "lib/myFunc/Random.h"
#include "lib/myFunc/MathFunc.h"

#include <externals/imgui/imgui.h>

BaseParticle::BaseParticle(){
	particles_.clear();
}

void BaseParticle::Initialize(const std::string& modelName, const std::string& texturePath, const uint32_t count){
	// 初期エミッターを1つ作って即時Emit（任意）
	ParticleData::Emitter emitter;
	emitter.Initialize(count);
	emitters_.push_back(emitter);

	modelName_ = modelName;
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

	if (autoEmit_){
		for (auto& emitter : emitters_){
			emitter.frequencyTime += deltaTime;
			if (emitter.frequencyTime >= emitter.frequency){
				Emit(emitter);
				emitter.frequencyTime = 0.0f;
			}
		}
	}

	// パーティクル更新
	instanceDataList_.clear();
	instanceNum_ = 0;

	for (auto it = particles_.begin(); it != particles_.end();){
		if (instanceNum_ >= kMaxInstanceNum_) break;

		if (it->lifeTime <= it->currentTime){
			it = particles_.erase(it);
			continue;
		}

		// 行列計算
		Matrix4x4 worldMatrix;
		if (isBillboard_){
			Matrix4x4 billboard = Matrix4x4::Multiply(backToFrontMatrix_, CameraManager::GetWorldMatrix());
			billboard.m[3][0] = billboard.m[3][1] = billboard.m[3][2] = 0.0f;
			worldMatrix = Matrix4x4::Multiply(
				Matrix4x4::Multiply(MakeScaleMatrix(it->transform.scale), billboard),
				MakeTranslateMatrix(it->transform.translate));
		} else{
			worldMatrix = Matrix4x4::Multiply(
				Matrix4x4::Multiply(MakeScaleMatrix(it->transform.scale), EulerToMatrix(it->transform.rotate)),
				MakeTranslateMatrix(it->transform.translate));
		}

		Matrix4x4 wvp = Matrix4x4::Multiply(worldMatrix, CameraManager::GetViewProjectionMatrix());

		// GPU転送用インスタンスデータ作成
		ParticleData::ParticleForGPU instance;
		instance.wvp = wvp;
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

void BaseParticle::Draw(){
	ComPtr<ID3D12GraphicsCommandList> commandList = GraphicsGroup::GetInstance()->GetCommandList();

	if (!modelData_) return;
	if (instanceNum_ == 0) return;

	modelData_->vertexBuffer.SetCommand(commandList);

	materialBuffer_.SetCommand(commandList, 0);
	//t0
	commandList->SetGraphicsRootDescriptorTable(1, instancingBuffer_.GetGpuHandle());
	//t1
	commandList->SetGraphicsRootDescriptorTable(2, textureHandle); 

	// 描画コマンド（インスタンシング）
	commandList->DrawInstanced(
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
	ImGui::Text(textureName_.c_str()); // ※ 新しくメンバ変数にしてもOK

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

	//サイズの設定
	ImGui::SeparatorText("Particle Size");
	ImGui::Checkbox("Use Random Scale", &useRandomScale_);
	if (useRandomScale_){
		ImGui::DragFloat("Min Scale", &randomScaleMin_, 0.01f, 0.0f, 10.0f);
		ImGui::DragFloat("Max Scale", &randomScaleMax_, 0.01f, 0.0f, 10.0f);
		// minとmaxの順序を保証
		if (randomScaleMin_ > randomScaleMax_){
			std::swap(randomScaleMin_, randomScaleMax_);
		}
	} else{
		ImGui::DragFloat("Max Scale", &fixedMaxScale_, 0.01f, 0.0f, 10.0f);
	}
}

void BaseParticle::EmitterGui(){
	if (ImGui::CollapsingHeader("Emitters", ImGuiTreeNodeFlags_DefaultOpen)){
		int emitterIndex = 0;
		for (auto& emitter : emitters_){
			ImGui::PushID(emitterIndex);
			std::string label = "Emitter " + std::to_string(emitterIndex);
			if (ImGui::TreeNode(label.c_str())){
				int shapeIndex = static_cast< int >(emitter.shape);
				const char* shapeNames[] = {"OBB", "Sphere"};
				if (ImGui::Combo("Shape", &shapeIndex, shapeNames, IM_ARRAYSIZE(shapeNames))){
					emitter.shape = static_cast< EmitterShape >(shapeIndex);
				}
				ImGui::DragFloat3("Position", &emitter.transform.translate.x, 0.01f);
				ImGui::DragFloat3("Scale", &emitter.transform.scale.x, 0.01f);
				ImGui::DragFloat("Frequency", &emitter.frequency, 0.01f, 0.0f);
				ImGui::DragInt("Count", reinterpret_cast< int* >(&emitter.count), 1, 1, 100);
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


void BaseParticle::Emit(ParticleData::Emitter& emitter){
	for (uint32_t i = 0; i < emitter.count && particles_.size() < static_cast< size_t >(kMaxInstanceNum_); ++i){
		ParticleData::Parameters particle;
		if (GetUseRandomColor()){
			particle.SetColorRandom();
		} else{
			particle.color = GetSelectedColor();
		}
		float speed = Random::Generate(0.5f, 2.0f);
		Vector3 localPoint = emitter.transform.translate; // 例：中心から
		particle.transform.translate = localPoint;
		particle.velocity = GenerateVelocity(speed);
		particle.lifeTime = SetParticleLifeTime();
		particle.transform.scale = useRandomScale_ ? Random::GenerateVector3(randomScaleMin_, randomScaleMax_)
			: Vector3(fixedMaxScale_);
		if (!isBillboard_){
			Matrix4x4 cam = CameraManager::GetWorldMatrix();
			particle.transform.rotate = Matrix4x4::ToEuler(cam);
		}
		particles_.push_back(particle);
	}
	instanceNum_ = static_cast< int32_t >(particles_.size());
}

Vector3 BaseParticle::GenerateVelocity(float speed){
	// デフォルトのランダムな方向の速度生成
	Vector3 velocity = Random::GenerateVector3(-1.0f, 1.0f);
	return velocity * speed;
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

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
	Emit(count);

	modelName_ = modelName;

	textureHandle = TextureManager::GetInstance()->LoadTexture(texturePath);

	backToFrontMatrix_ = MakeRotateYMatrix(std::numbers::pi_v<float>);
	instanceDataList_.reserve(kMaxInstanceNum_);

	CreateBuffer();
}

void BaseParticle::Update(){
	const float deltaTime = ClockManager::GetInstance()->GetDeltaTime();

	// ----------------------------------------
	// ▼ モデルが未取得ならチェック
	// ----------------------------------------
	if (!modelData_){
		if (ModelManager::GetInstance()->IsModelLoaded(modelName_)){
			modelData_ = ModelManager::GetInstance()->GetModelData(modelName_);

			ComPtr<ID3D12Device>device = GraphicsGroup::GetInstance()->GetDevice();
			//=== 頂点バッファ初期化 ===//
			modelData_->vertexBuffer.Initialize(device, UINT(modelData_->vertices.size()));
			modelData_->vertexBuffer.TransferVectorData(modelData_->vertices);
		}
	} else{
		// ----------------------------------------
		// ▼ パーティクルの更新
		// ----------------------------------------
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

		// ----------------------------------------
		// ▼ GPUバッファ転送
		// ----------------------------------------
		if (instanceNum_ > 0){
			instancingBuffer_.TransferVectorData(instanceDataList_);
		}
		modelData_->vertexBuffer.TransferVectorData(modelData_->vertices);
		materialBuffer_.TransferData(materialData_);
		// ----------------------------------------
		// ▼ 自動生成
		// ----------------------------------------
		if (autoEmit_){
			emitter_.frequencyTime += deltaTime;
			if (emitter_.frequencyTime >= emitter_.frequency){
				Emit(emitter_.count);
				emitter_.frequencyTime = 0.0f;
			}
		}
	}


}

void BaseParticle::Draw(){
	ComPtr<ID3D12GraphicsCommandList> commandList = GraphicsGroup::GetInstance()->GetCommandList();

	if (!modelData_) return;
	if (instanceNum_ == 0) return;

	modelData_->vertexBuffer.SetCommand(commandList);

	// 定数バッファ（マテリアル）をルートパラメータ1にバインド
	materialBuffer_.SetCommand(commandList, 0);

	// インスタンスバッファ（StructuredBuffer SRV）をルートパラメータ2にバインド
	commandList->SetGraphicsRootDescriptorTable(1, instancingBuffer_.GetGpuHandle());

	// テクスチャ（SRV）をルートパラメータ3にバインド（想定：ImTextureIDをSRVとして使っている場合）
	commandList->SetGraphicsRootDescriptorTable(2, textureHandle); // 以前と同様

	// 描画コマンド（インスタンシング）
	commandList->DrawInstanced(
		static_cast< UINT >(modelData_->vertices.size()), // 頂点数
		instanceNum_,                                   // インスタンス数
		0, 0
	);

#ifdef _DEBUG
	// デバッグ用：エミッタの形状を描画
	if (currentShape_ == EmitterShape::OBB){
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
	}
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

		if (currentShape_ == EmitterShape::OBB){
			ImGui::DragFloat3("size", &emitter_.transform.scale.x, 0.01f);
			ImGui::Checkbox("Emit +X Face", &emitPosX_);
			ImGui::Checkbox("Emit -X Face", &emitNegX_);
			ImGui::Checkbox("Emit +Y Face", &emitPosY_);
			ImGui::Checkbox("Emit -Y Face", &emitNegY_);
			ImGui::Checkbox("Emit +Z Face", &emitPosZ_);
			ImGui::Checkbox("Emit -Z Face", &emitNegZ_);
		}

		if (currentShape_ == EmitterShape::Sphere){
			ImGui::DragFloat("radius", &emitter_.transform.scale.x, 0.01f);
		}
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
	const auto& models = ModelManager::GetInstance()->GetLoadedModelNames(); // 仮にこの関数があれば
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

}

void BaseParticle::EmitterGui(){
	if (ImGui::CollapsingHeader("emitterType")){

		// 形状選択UI
		int shapeIndex = ( int ) currentShape_;
		const char* shapeNames[] = {"OBB", "Sphere"};
		if (ImGui::Combo("Emitter Shape", &shapeIndex, shapeNames, IM_ARRAYSIZE(shapeNames))){
			currentShape_ = ( EmitterShape ) shapeIndex;
		}

		if (currentShape_ == EmitterShape::OBB){
			ImGui::DragFloat3("size", &emitter_.transform.scale.x, 0.01f);
			ImGui::Checkbox("Emit +X Face", &emitPosX_);
			ImGui::Checkbox("Emit -X Face", &emitNegX_);
			ImGui::Checkbox("Emit +Y Face", &emitPosY_);
			ImGui::Checkbox("Emit -Y Face", &emitNegY_);
			ImGui::Checkbox("Emit +Z Face", &emitPosZ_);
			ImGui::Checkbox("Emit -Z Face", &emitNegZ_);
		}

		if (currentShape_ == EmitterShape::Sphere){
			ImGui::DragFloat("radius", &emitter_.transform.scale.x, 0.01f);
		}
	}
}

void BaseParticle::Emit(uint32_t count){
	for (uint32_t i = 0; i < count; ++i){
		if (particles_.size() >= static_cast< size_t >(kMaxInstanceNum_)){
			break;
		}

		ParticleData::Parameters particle;
		if (GetUseRandomColor()){
			particle.SetColorRandom();
		} else{
			particle.color = GetSelectedColor();
		}

		float speed = Random::Generate(0.5f, 2.0f);

		Vector3 localPoint {};
		Vector3 localNormal {};

		// パーティクルの初期位置と速度の設定
		if (currentShape_ == EmitterShape::OBB){
			FaceInfo fi = GetRandomPointAndNormalOnOBBSurface(emitter_.transform,
															  emitPosX_, emitNegX_,
															  emitPosY_, emitNegY_,
															  emitPosZ_, emitNegZ_);
			localPoint = fi.localPoint;
			localNormal = fi.localNormal;
			particle.velocity = TransformNormal(localNormal, Matrix4x4::MakeIdentity()) * speed;

		} else if (currentShape_ == EmitterShape::Sphere){
			Vector3 si = GetRandomPointOnSphere(emitter_.transform);
			localPoint = si;
			particle.SetVelocityRandom(-1.0f, 1.0f);
		}

		Vector3 worldPos = Vector3::Transform(localPoint, Matrix4x4::MakeIdentity()) + emitter_.transform.translate;
		particle.transform.translate = worldPos;
		particle.maxScale = fixedMaxScale_;
		// maxScaleの設定（ランダムまたは固定値）
		if (useRandomScale_){

			particle.transform.scale = Random::GenerateVector3(randomScaleMin_, randomScaleMax_); // ランダム値を設定
		} else{
			particle.transform.scale = Vector3(fixedMaxScale_, fixedMaxScale_, fixedMaxScale_); // 固定値を設定
		}

		if (!isBillboard_){
			// 非ビルボードモードでは発生時にカメラの方向を設定
			Matrix4x4 cameraMatrix = CameraManager::GetWorldMatrix();
			particle.transform.rotate = Matrix4x4::ToEuler(cameraMatrix);
		}

		// ライフタイムの設定
		particle.lifeTime = SetParticleLifeTime();
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

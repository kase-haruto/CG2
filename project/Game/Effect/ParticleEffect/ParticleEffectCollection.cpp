#include "ParticleEffectCollection.h"
/* ========================================================================
/*	include space
/* ===================================================================== */
#include <Engine/graphics/GraphicsGroup.h>
// externals
#include <externals/imgui/imgui.h>

///////////////////////////////////////////////////////////////////////////////////////////
//		更新
///////////////////////////////////////////////////////////////////////////////////////////
void ParticleEffectCollection::Update(){
	for (auto& effect : effects_){
		effect->Update();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////
//		描画
///////////////////////////////////////////////////////////////////////////////////////////
void ParticleEffectCollection::Draw(){
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>commandList = GraphicsGroup::GetInstance()->GetCommandList();

	// プリミティブトポロジーを設定（ここでは三角形リストを指定）
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	for (auto& effect : effects_){
		BlendMode blendMode = BlendMode::ADD;
		Microsoft::WRL::ComPtr<ID3D12RootSignature>rootSignature = GraphicsGroup::GetInstance()->GetRootSignature(StructuredObject, blendMode);
		Microsoft::WRL::ComPtr<ID3D12PipelineState>pipelineState = GraphicsGroup::GetInstance()->GetPipelineState(StructuredObject, blendMode);

		// ルートシグネチャを設定
		commandList->SetGraphicsRootSignature(rootSignature.Get());

		// パイプラインステートを設定
		commandList->SetPipelineState(pipelineState.Get());

		effect->Draw();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////
//		imgui
///////////////////////////////////////////////////////////////////////////////////////////
void ParticleEffectCollection::ImGui(){
	ImGui::Begin("Particle Effect Collection");
	for (size_t i = 0; i < effects_.size(); ++i){
		ImGui::PushID(static_cast< int >(i));
		if (ImGui::CollapsingHeader(("Effect ##" + std::to_string(i)).c_str(), ImGuiTreeNodeFlags_DefaultOpen)){
			effects_[i]->ImGui();
			if (ImGui::Button("Remove Effect")){
				RemoveEffect(i);
				ImGui::PopID();
				break;
			}
		}
		ImGui::PopID();
	}
	if (ImGui::Button("Add Effect")){
		auto effect = std::make_unique<ParticleEffect>();
		AddEffect(std::move(effect));
	}
	ImGui::End();
}


///////////////////////////////////////////////////////////////////////////////////////////
//		追加
///////////////////////////////////////////////////////////////////////////////////////////
void ParticleEffectCollection::AddEffect(std::unique_ptr<ParticleEffect> effect){
	effects_.emplace_back(std::move(effect));
}

///////////////////////////////////////////////////////////////////////////////////////////
//		削除
///////////////////////////////////////////////////////////////////////////////////////////
void ParticleEffectCollection::RemoveEffect(size_t index){
	effects_.erase(effects_.begin() + index);
}

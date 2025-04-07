#include "ParticleEditor.h"
/* ========================================================================
/* include space
/* ===================================================================== */

/* engine */
#include <Engine/objects/particle/ParticleManager.h>

/* externals */
#include <externals/imgui/imgui.h>

/////////////////////////////////////////////////////////////////////////////////////////
//		コンストラクタ
/////////////////////////////////////////////////////////////////////////////////////////
ParticleEditor::ParticleEditor(){
	// コンストラクタの初期化
	editorName_ = "ParticleEditor";
}

ParticleEditor::ParticleEditor(ParticleManager* manager)
	:particleManager_(manager){
	editorName_ = "ParticleEditor";
}

/////////////////////////////////////////////////////////////////////////////////////////
//		gui表示
/////////////////////////////////////////////////////////////////////////////////////////
void ParticleEditor::ShowImGuiInterface(){
#ifdef _DEBUG

	//追加するパーティクルの名前を入力
	static char newParticleName[128] = ""; // デフォルトのパーティクル名
	ImGui::InputText("Particle Name", newParticleName, sizeof(newParticleName));
	
	// emitterの形状を選択
	const char* emitterShapes[] = {"Sphere", "Box"};


	// パーティクルの追加ボタン
	if (ImGui::Button("Add Particle")){
		// パーティクルの追加処理
		ParticleSystem* newParticle = new ParticleSystem(newParticleName);
		newParticle->Initialize("particleModel", "particleTexture");
		particleManager_->AddSystem(newParticle);
	}

#endif // _DEBUG
}

/////////////////////////////////////////////////////////////////////////////////////////
//		jsonに保存
/////////////////////////////////////////////////////////////////////////////////////////
void ParticleEditor::SaveToJson([[maybe_unused]]const std::string& filePath){

}

/////////////////////////////////////////////////////////////////////////////////////////
//		jsonからロード
/////////////////////////////////////////////////////////////////////////////////////////
void ParticleEditor::LoadFromJson([[maybe_unused]] const std::string& filePath){

}

#pragma once
#include"WinApp.h"

//グラフィック関係
#include "engine/core/DirectX/DxCore.h"
#include "../graphics/ShaderManager.h"
#include "../graphics/PipelineStateManager.h"

#include "lib/ImGuiManager.h"
#include <Game/Effect/ParticleEffect/ParticleEffectCollection.h>
#include <Engine/Editor/EffectEditor.h>

//リークチェック
#include "LeakChecker.h"

/* c++ */
#include<stdint.h>

// forward
class UIEditor;
class ModelBuilder;

class System{
public:
	//===================================================================*/
	//                    public functions
	//===================================================================*/
	System();
	~System() = default;

	void Initialize(HINSTANCE hInstance, int32_t clientWidth, int32_t clientHeight,const std::string windowTitle);
	void InitializeEngineUI();
	void Finalize();
	void BeginFrame();
	void EndFrame();

	void EditorUpdate();	//engine内部Editorの更新
	void EditorDraw();		//engine内部editorの描画

	int ProcessMessage();

	//* パイプラインの作成 ==============================*/
	void CreatePipelines();
	void Object3DPipelines();
	void SkinningObject3dPipeline();
	void Object2DPipelines();
	void StructuredObjectPipeline();
	void LinePipeline();
	void CopyImagePipeline();
	void EffectPipeline();

	//===================================================================*/
	//                    getter / setter
	//===================================================================*/
	static HINSTANCE GetHinstance(){ return hInstance_; }
	static HWND GetHWND(){ return hwnd_; }
	DxCore* GetDxCore()const{ return dxCore_.get(); }

private:
	//===================================================================*/
	//                    private members
	//===================================================================*/
	LeakChecker leakChecker_;
	std::unique_ptr<DxCore> dxCore_ = nullptr;

	/*window*/
	std::unique_ptr<WinApp> winApp_;	//ウィンドウ
	static HINSTANCE hInstance_;		//インスタンス
	static HWND hwnd_;					//ウィンドウハンドル

	// ImGuiの初期化
	std::unique_ptr<ImGuiManager> imguiManager_ = nullptr;

private:
	// grapics
	std::shared_ptr<ShaderManager>shaderManager_;					//shader管理
	std::unique_ptr<PipelineStateManager>pipelineStateManager_;		//パイプライン管理

private:
	// engineEditors
	std::unique_ptr<UIEditor> uiEditor_;			//ui編集
	std::unique_ptr<ModelBuilder> modelBuilder_;	//モデル配置
	std::unique_ptr<EffectEditor> effectEditor_;	//パーティクルエディタ
	
	// particle
	std::unique_ptr<ParticleEffectCollection> particleEffectCollection_;
};


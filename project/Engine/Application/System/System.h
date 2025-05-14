#pragma once

/* ========================================================================
/*	include space
/* ===================================================================== */
// engine
#include <Engine/Application/UI/ImGuiManager.h>
#include <Engine/Assets/Model/ModelBuilder.h>
#include <Engine/Editor/EffectEditor.h>
#include <Engine/Editor/UiEditor.h>
#include <Engine/Graphics/Device/DxCore.h>
#include <Engine/Graphics/Pipeline/Manager/PipelineStateManager.h>
#include <Game/Effect/ParticleEffect/ParticleEffectCollection.h>

// postprocess
#include <Engine/PostProcess/Collection/PostProcessCollection.h>
#include <Engine/PostProcess/Graph/PostEffectGraph.h>
#include <Engine/PostProcess/Slot/PostEffectSlot.h>

//リークチェック
#include <Engine/Foundation/Utility/LeakChecker/LeakChecker.h>

/* c++ */
#include<stdint.h>

class System{
public:
	//===================================================================*/
	//                    public functions
	//===================================================================*/
	System();
	~System() = default;

	void Initialize(HINSTANCE hInstance, int32_t clientWidth, int32_t clientHeight,const std::string _windowTitle);
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
	void EffectPipeline();
	void SkyBoxPipeline();

	void CopyImagePipeline();
	void GrayScalePipeline();
	void RadialBlurPipeline();
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
	
	// postprocess
	std::unique_ptr<PostProcessCollection> postProcessCollection_;
	std::unique_ptr<PostEffectGraph> postEffectGraph_;
	std::vector<PostEffectSlot> postEffectSlots_;

	float radialTimer_ = 0.0f;
	const float kRadialDurationSec_ = 1.0f;
	bool isRadialActive_ = false;
};


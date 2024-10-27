#pragma once
#include"WinApp.h"

//グラフィック関係
#include"core/DirectX/DxCore.h"
#include"ShaderManager.h"
#include"PipelineStateManager.h"
#include"graphics/FogEffect.h"

//オブジェクト
#include"DirectionalLight.h"
#include"PointLight.h"

#include"ImGuiManager.h"

//リークチェック
#include "LeakChecker.h"


#include<stdint.h>

class System{
public:
	System();
	~System() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="clientWidth"></param>
	/// <param name="clientHeight"></param>
	void Initialize(HINSTANCE hInstance, int32_t clientWidth, int32_t clientHeight,const std::string windowTitle);
	/// <summary>
	/// メモリの開放など
	/// </summary>
	void Finalize();
	/// <summary>
	/// フレーム前処理
	/// </summary>
	void BeginFrame();
	/// <summary>
	/// フレーム後処理
	/// </summary>
	void EndFrame();

	/// <summary>
	/// rootSignatureの設定を行う
	/// </summary>
	void SettingRootSignatur1es();

	/// <summary>
	/// pipelineの設定を行う
	/// </summary>
	void CreatePipelines();
	void Object3DPipelines();
	void Object2DPipelines();
	void StructuredObjectPipeline();
	void LinePipeline();


	int ProcessMessage();


	////////////////////////////////////////////////////////////
	//		アクセッサ
	///////////////////////////////////////////////////////////
	static HINSTANCE GetHinstance(){ return hInstance_; }
	static HWND GetHWND(){ return hwnd_; }

private:
	LeakChecker leakChecker_;
	std::unique_ptr<DxCore> dxCore_ = nullptr;

	/*window*/
	std::unique_ptr<WinApp> winApp_;
	static HINSTANCE hInstance_;
	static HWND hwnd_;

	std::unique_ptr<FogEffect>fog = nullptr;

	// ImGuiの初期化
	std::unique_ptr<ImGuiManager> imguiManager_ = nullptr;

	/// <summary>
	/// グラフィック関連管理クラス
	/// </summary>
	std::shared_ptr<ShaderManager>shaderManager_;
	std::unique_ptr<PipelineStateManager>pipelineStateManager_;

	/// <summary>
	/// オブジェクト関連クラス
	/// </summary>
	std::unique_ptr<DirectionalLight>directionalLight_;
	std::unique_ptr<PointLight> pointLight_;

};


#pragma once
#include"WinApp.h"

//グラフィック関係
#include"DirectXCommon.h"
#include"ShaderManager.h"
#include"PipelineStateManager.h"

//オブジェクト
#include"DirectionalLight.h"

#ifdef _DEBUG
#include"ImGuiManager.h"
#endif // _DEBUG


#include<stdint.h>

class System{
public:
	System();
	~System();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="clientWidth"></param>
	/// <param name="clientHeight"></param>
	void Initialize(int32_t clientWidth,int32_t clientHeight);
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
	void NonTexturesObjectPiplines();


	int ProcessMessage();


private:
	std::unique_ptr<WinApp> winApp_;
	std::unique_ptr<DirectXCommon> dxCommon_;
	ComPtr<ID3D12Device> device_;
#ifdef _DEBUG
	// ImGuiの初期化
	std::unique_ptr<ImGuiManager> imguiManager_ = nullptr;
#endif // _DEBUG

	/// <summary>
	/// グラフィック関連管理クラス
	/// </summary>
	std::shared_ptr<ShaderManager>shaderManager_;
	std::unique_ptr<PipelineStateManager>pipelineStateManager_;

	/// <summary>
	/// オブジェクト関連クラス
	/// </summary>
	std::unique_ptr<DirectionalLight>light_;
};


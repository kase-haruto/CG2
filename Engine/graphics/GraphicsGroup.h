#pragma once
#include"PipelineStateManager.h"
#include"DirectXCommon.h"

class GraphicsGroup{
private:
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
	static GraphicsGroup* GetInstance();
	~GraphicsGroup() = default;

	/// <summary>
	/// Graphic関連初期化
	/// </summary>
	/// <param name="dxCommon"></param>
	/// <param name="psManager"></param>
	void Initialize(DirectXCommon* dxCommon,PipelineStateManager* psManager);

	///=========================================
	/// pipelineの取得
	///=========================================
	PipelineStateManager* GetPipelineState()const;
	const ComPtr<ID3D12RootSignature>& GetRootSignature(const PipelineType& type)const;
	const ComPtr<ID3D12PipelineState>& GetPipelineState(const PipelineType& type)const;

	///=========================================
	/// device/commandListの取得
	///=========================================
	const ComPtr<ID3D12Device>& GetDevice()const;
	const ComPtr<ID3D12GraphicsCommandList>& GetCommandList()const;

private:
	GraphicsGroup() :dxCommon_(nullptr), pipelineManager_(nullptr){}
	GraphicsGroup(const GraphicsGroup&) = delete;
	GraphicsGroup& operator = (const GraphicsGroup*) = delete;

private:
	DirectXCommon* dxCommon_;
	PipelineStateManager* pipelineManager_;
};


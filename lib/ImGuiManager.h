#pragma once

#ifdef _DEBUG
#include<imgui.h>
#endif // _DEBUG

#include<d3d12.h>
#include <wrl.h>



class WinApp;
class DxCore;

class ImGuiManager{
public:

	ImGuiManager() = default;
	~ImGuiManager() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="winApp"></param>
	/// <param name="dxCore"></param>
	void Initialize(WinApp* winApp, const DxCore* dxCore);

	/// <summary>
	/// 終了
	/// </summary>
	void Finalize();

	/// <summary>
	/// ImGui受付開始
	/// </summary>
	void Begin();

	/// <summary>
	/// ImGui受付終了
	/// </summary>
	void End();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	
	//Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> GetSrvHeap()const{ return srvHeap_; }

private:
#ifdef _DEBUG
	// DirectX基盤インスタンス（借りてくる）
	const DxCore* pDxCore_ = nullptr;

	// SRV用ヒープ
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvHeap_;

#endif // _DEBUG

};


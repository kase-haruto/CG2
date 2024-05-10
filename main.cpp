#include<Windows.h>
#include"WinApp.h"
#include"DirectXCommon.h"
#include"TextureManager.h"
#include"FogEffect.h"
#include"ViewProjection.h"
#include"ImGuiManager.h"
#include<imgui.h>


int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int){
	//comの初期化
	CoInitializeEx(0, COINIT_MULTITHREADED);

	WinApp* win = win->GetInstance();
	MSG msg {};
	DirectXCommon* dxCommon = dxCommon->GetInstance();
	dxCommon->Initialize(win, 1280, 720);
	dxCommon->Pipeline();

	FogEffect* fog = new FogEffect(dxCommon);


#pragma region 汎用機能初期化

#ifdef _DEBUG
	// ImGuiの初期化
	ImGuiManager* imguiManager = ImGuiManager::GetInstance();
	imguiManager->Initialize(win, dxCommon);
#endif // _DEBUG

#pragma endregion

	TextureManager::GetInstance()->Initialize(dxCommon->GetDevice(), imguiManager);
	DirectX::ScratchImage mipImages = TextureManager::LoadTexture("./Resources/uvChecker.png");
	const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
	ID3D12Resource* textureResource = TextureManager::CreateTextureResource(dxCommon->GetDevice(), metadata);
	TextureManager::UploadTextureData(textureResource, mipImages);
	TextureManager::GetInstance()->CreateShaderResourceView(textureResource, metadata);

	while (win->ProcessMessage() == 0){

		// ImGui受付開始
		imguiManager->Begin();
		//開発用UIの処理
		dxCommon->ImGui();
		//三角形の更新
		dxCommon->UpdatePolygon();
		// ImGui受付終了
		imguiManager->End();




		//フレームの開始
		dxCommon->PreDraw();
		//ImGui描画
		imguiManager->Draw();
		//三角形の描画
		dxCommon->DrawPolygon();
		//フレームの終了
		dxCommon->PostDraw();
	}

	// ImGui解放
	imguiManager->Finalize();
	dxCommon->Finalize();
	delete win, fog;
	CoUninitialize();
	return 0;
}
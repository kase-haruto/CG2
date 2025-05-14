#pragma once

/* ========================================================================
/*  include space
/* ===================================================================== */

// engine
#include <Engine/Application/UI/EngineUI/IEngineUI.h>
#include <Engine/Objects/3D/Actor/SceneObject.h>

// c++
#include <vector>
#include <memory>
#include <functional>
#include <d3d12.h>

class EngineUI{
public:
    //===================================================================*/
    //                    singleton
    //===================================================================*/
    EngineUI(const EngineUI&) = delete;
    EngineUI& operator=(const EngineUI&) = delete;
    static EngineUI* GetInstance(); // インスタンスの取得

public:
    //===================================================================*/
    //                    public function
    //===================================================================*/
    static void Initialize();
    static void Render();
    static void Finalize();

    void AddPanel(std::unique_ptr<IEngineUI> panel);
    void RemovePanel(const std::string& panelName);

    static void SetMainViewportTexture(UINT64 textureID);
	static void SetDebugViewportTexture(UINT64 textureID);

    //パネルの取得
	template<class Panel>
	Panel* GetPanel(){
		for (auto& panel : panels_){
			if (auto p = dynamic_cast< Panel* >(panel.get())){
				return p;
			}
		}
		return nullptr;
	}

private:
    //===================================================================*/
    //                    private function
    //===================================================================*/
    EngineUI() = default;       // コンストラクタ
    ~EngineUI() = default;      // デストラクタ

    void RenderMainViewport();  // メインビューポートの描画
	void RenderDebugViewPort(); // デフォルトビューポートの描画
	void RenderMenue();         // メニューの描画

private:
	//===================================================================*/
	//                    private variable
	//===================================================================*/
    static EngineUI* pInstance_;
    std::vector<std::unique_ptr<IEngineUI>> panels_;

    UINT64 mainViewportTextureID_ = 0;
	UINT64 debugViewportTextureID_ = 0;

};


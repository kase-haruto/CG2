#pragma once


// EngineUI.h
#ifndef ENGINE_UI_H
#define ENGINE_UI_H

#include <string>
#include <functional>
#include <vector>

class EngineUI{
public:
    //===================================================================*/
    //                    singleton
    //===================================================================*/
    //コピー参照の禁止
	EngineUI(const EngineUI&) = delete;
	EngineUI& operator=(const EngineUI&) = delete;
	//インスタンスの取得
    static EngineUI* GetInstance();

public:
    //===================================================================*/
	//                    public function
    //===================================================================*/

    // 初期化
    static void Initialize();
    // UIのレンダリング
    static void Render();
	// 終了処理
    static void Finalize();

    // 各UIセクションのコールバック設定
    static void SetMainViewportCallback(std::function<void()> callback);
    static void SetToolbarCallback(std::function<void()> callback);
    static void AddPanelCallback(const std::string& panelName, std::function<void()> callback);
    static void SetFloatingWindowCallback(std::function<void()> callback);

private:
    //===================================================================*/
	//                   private function
    //===================================================================*/
    EngineUI() = default;
    ~EngineUI() = default;

private:
    // コールバック関数
    std::function<void()> mainViewportCallback_;
    std::function<void()> toolbarCallback_;
    std::vector<std::pair<std::string, std::function<void()>>> panelCallbacks_;
    std::function<void()> floatingWindowCallback_;

    // 内部描画メソッド
    void RenderDockSpace();
    void RenderToolbar();
    void RenderPanels();
    void RenderMainViewport();
    void RenderFloatingWindows();

private:
    //===================================================================*/
	//                   private variable
    //===================================================================*/
	static EngineUI* pInstance_;
};

#endif // ENGINE_UI_H


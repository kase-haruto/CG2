#pragma once


// EngineUI.h
#ifndef ENGINE_UI_H
#define ENGINE_UI_H

#include <string>
#include <functional>
#include <vector>

class EngineUI{
public:
    EngineUI() = default;
    ~EngineUI() = default;

    // 初期化
    void Initialize();

    // UIのレンダリング
    void Render();

    // 各UIセクションのコールバック設定
    void SetMainViewportCallback(std::function<void()> callback);
    void SetToolbarCallback(std::function<void()> callback);
    void AddPanelCallback(const std::string& panelName, std::function<void()> callback);
    void SetFloatingWindowCallback(std::function<void()> callback);

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
};

#endif // ENGINE_UI_H


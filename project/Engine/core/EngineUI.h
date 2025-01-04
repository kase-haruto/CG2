#pragma once

#pragma once
#include <vector>
#include <memory>
#include <functional>
#include <d3d12.h>
#include "UI/IEngineUI.h"
#include "Engine/objects/SceneObject.h"

///===================================================================*/
/// EngineUIクラス
///===================================================================*/
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
    /// <summary>
    /// 初期化処理
    /// </summary>
    static void Initialize();

    /// <summary>
    /// UIのレンダリング
    /// </summary>
    static void Render();

    /// <summary>
    /// 終了処理
    /// </summary>
    static void Finalize();

    /// <summary>
    /// 新しいパネルを追加する
    /// </summary>
    void AddPanel(std::unique_ptr<IEngineUI> panel);

    /// <summary>
  /// パネルを削除する
  /// </summary>
  /// <param name="panelName">削除するパネルの名前</param>
    void RemovePanel(const std::string& panelName);

    // メインビューポート用のテクスチャを設定
    static void SetMainViewportTexture(UINT64 textureID);

private:
    //===================================================================*/
    //                    private function
    //===================================================================*/
    EngineUI() = default;       // コンストラクタ
    ~EngineUI() = default;      // デストラクタ

    void RenderMainViewport();  // メインビューポートの描画
	void RenderMenue();         // メニューの描画

private:
	//===================================================================*/
	//                    private variable
	//===================================================================*/
    static EngineUI* pInstance_;
    std::vector<std::unique_ptr<IEngineUI>> panels_;

    UINT64 mainViewportTextureID_ = 0;

    // AttackEditorパネルの表示状態と管理用ポインタ
    bool showPlayerAttackEditor_ = false;
};


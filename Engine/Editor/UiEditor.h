#pragma once

/* engine */
#include "engine/Editor/BaseEditor.h"
#include "lib/myMath/Vector2.h"

/* lib */
#include <vector>
#include <string>
#include <memory>

/* 先方宣言 */
class TextureManager;
class Sprite;

/// <summary>
/// spriteEditor
/// </summary>
class UIEditor final:
public BaseEditor{
public:
    UIEditor();   // コンストラクタ
    ~UIEditor() = default;  //デストラクタ

    // UIの描画
    void ShowImGuiInterface()override;


    // 更新処理
    void Update();

    // スプライトの描画
    void Draw();

    const char* GetEditorName() const override{
        return "UiEditor";
    }

private:
    void AddSprite(const std::string& textureName, const Vector2& position, const Vector2& size);

private:
    // テクスチャマネージャーのインスタンス
    TextureManager* textureManager_;

    // スプライトのリスト
    std::vector<std::shared_ptr<Sprite>> sprites_;

    // 現在選択されているスプライトのインデックス
    int selectedSpriteIndex_ = -1;
};

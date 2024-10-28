#pragma once

/// <summary>
/// editor
/// </summary>
class BaseEditor{
public:
    virtual void ShowImGuiInterface() = 0; // 純粋仮想関数
    virtual ~BaseEditor() = default;

    // エディタ名を取得する仮想関数
    virtual const char* GetEditorName() const = 0;
};
#pragma once

#include <string>

/// <summary>
/// editor
/// </summary>
class BaseEditor{
public:
    virtual void ShowImGuiInterface() = 0; // 純粋仮想関数
    virtual ~BaseEditor() = default;

    // エディタ名を取得する仮想関数
    const std::string& GetEditorName() const;

protected:
	std::string editorName_; // エディタ名
};
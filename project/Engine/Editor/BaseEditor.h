#pragma once

#include <string>

class BaseEditor{
public:
	BaseEditor(const std::string& name) : editorName_(name){}
	virtual ~BaseEditor() = default;
	virtual void ShowImGuiInterface() = 0; // 純粋仮想関数

	const std::string& GetEditorName() const{return editorName_; }

protected:
	std::string editorName_ = "Editor"; // エディタ名
};
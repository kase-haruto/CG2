#pragma once

#include <string>

/*--------------------------------------
 UIパネルのインターフェース
 すべてのパネルがこのインターフェースを実装
--------------------------------------*/
class IEngineUI{
public:
	//===================================================================*/
	//                   public methods
	//===================================================================*/
	IEngineUI(const std::string& name);
	IEngineUI();
	virtual ~IEngineUI() = default;

	// パネルの描画
	virtual void Render() = 0;

	// パネル名を取得
	virtual const std::string& GetPanelName() const = 0;

	void SetShow(bool isShow){ isShow_ = isShow; }
	bool IsShow() const{ return isShow_; }

protected:
	//===================================================================*/
	//                   protected methods
	//===================================================================*/
	std::string panelName_;
	bool isShow_ = true;
};
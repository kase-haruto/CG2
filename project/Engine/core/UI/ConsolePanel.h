#pragma once

#include "IEngineUI.h"
#include <vector>
#include <string>

/// <summary>
/// コンソールパネル
/// </summary>
class ConsolePanel
    : public IEngineUI{
public:
    //===================================================================*/
    //                   public funclion
    //===================================================================*/
    ConsolePanel();
    ~ConsolePanel() override = default;

    void Render() override;
    const std::string& GetPanelName() const override;

    void AddLog(const std::string& log);

private:
    //===================================================================*/
    //                   private variable
    //===================================================================*/
	int selectedLogType_ = 0;
};


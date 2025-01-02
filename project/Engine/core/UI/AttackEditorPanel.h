#pragma once

#include "IEngineUI.h"
#include "Game/3d/Player/PlayerAttack/PlayerAttackEditor.h"

#include <vector>
#include <string>
class AttackEditorPanel
:public IEngineUI{
public:
    //===================================================================*/
    //                   public funclion
    //===================================================================*/
    AttackEditorPanel();
    ~AttackEditorPanel() override = default;

    void Render() override;
    const std::string& GetPanelName() const override;

private:
	PlayerAttackEditor* playerAttackEditor_ = nullptr;

public:
	//===================================================================*/
	//                   getter/setter
	//===================================================================*/
	void SetPlayerAttackEditor(PlayerAttackEditor* playerAttackEditor){ playerAttackEditor_ = playerAttackEditor; }
};




#pragma once

// engine
#include "IEngineUI.h"
#include "Engine/objects/SceneObject.h"

// c++
#include <vector>
#include <string>
class EditorPanel
	:public IEngineUI{
public:
	//===================================================================*/
	//                   public functions
	//===================================================================*/
	EditorPanel();
	~EditorPanel() = default;

	void Render(); // �`�揈��
	const std::string& GetPanelName() const; // �p�l�����̎擾

private:
	//===================================================================*/
	//                   private variables
	//===================================================================*/

public:

};


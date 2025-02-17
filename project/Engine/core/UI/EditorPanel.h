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

	void Render(); // •`‰æˆ—
	const std::string& GetPanelName() const; // ƒpƒlƒ‹–¼‚Ìæ“¾

private:
	//===================================================================*/
	//                   private variables
	//===================================================================*/

public:

};


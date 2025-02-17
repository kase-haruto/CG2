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

	void Render(); // 描画処理
	const std::string& GetPanelName() const; // パネル名の取得

private:
	//===================================================================*/
	//                   private variables
	//===================================================================*/

public:

};


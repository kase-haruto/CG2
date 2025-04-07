#pragma once

/* ========================================================================
/* include space
/* ===================================================================== */
#include <Engine/Editor/BaseEditor.h>

/* forward */
class ParticleManager;

/* ========================================================================
/* particle editor
/* ===================================================================== */
class ParticleEditor 
	: public BaseEditor{
public:
	//===================================================================*/
	//                    public functions
	//===================================================================*/
	ParticleEditor();
	ParticleEditor(ParticleManager* manager);
	~ParticleEditor() override = default;

	void ShowImGuiInterface() override;				//< ImGuiの描画

	//--------- json -----------------------------------------------------
	void SaveToJson(const std::string& filePath);	//< json保存
	void LoadFromJson(const std::string& filePath);	//< json読み込み

private:
	//===================================================================*/
	//                    private functions
	//===================================================================*/

private:
	//===================================================================*/
	//                    private variables
	//===================================================================*/
	ParticleManager* particleManager_ = nullptr;	//< particleManager

	const std::string kJsonPath = "Resources/json/particle/particleData.json";	//< jsonのパス
};


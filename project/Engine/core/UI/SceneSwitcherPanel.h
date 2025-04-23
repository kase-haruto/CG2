#pragma once
#include "IEngineUI.h"
#include <Engine/scene/Interface/IScene.h>

#include <vector>
#include <utility>
#include <string>

class SceneManager;

class SceneSwitcherPanel : public IEngineUI{
public:
	SceneSwitcherPanel(SceneManager* sceneManager);

	~SceneSwitcherPanel() override = default;

	// パネルの描画 (ImGuiを使う想定)
	void Render() override;

	// シーンオプションを追加
	void AddSceneOption(const std::string& sceneName, SceneType sceneType){
		sceneOptions_.emplace_back(sceneName, sceneType);
	}

	const std::string& GetPanelName() const override{
		return panelName_;
	}

private:
	std::string panelName_ {"Scene Switcher Panel"};
	SceneManager* sceneManager_ = nullptr;

	// (A) シーン名と SceneType のペアを管理
	std::vector<std::pair<std::string, SceneType>> sceneOptions_;
};

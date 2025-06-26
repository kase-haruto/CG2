#pragma once
/* ========================================================================
/*      include space
/* ===================================================================== */
// engine
#include <Engine/Application/UI/EngineUI/IEngineUI.h>
#include <Engine/Scene/Context/ISceneContextListener.h>
#include <Engine/Foundation/Math/Vector2.h>

// c++
#include <memory>
#include <d3d12.h>
#include <string>
#include <unordered_map>
#include <vector>
#include <functional>

class SceneContext;
class SceneObject;

class PlaceToolPanel
	: public IEngineUI, public ISceneContextListener{
public:
	enum class ShapeObjType{
		Plane,
		Cube,
		Sphere,
		Cylinder,
		Cone,
		Torus,
		Count
	};

	enum class PlaceItemCategory{
		Shape,
		Light,
		Particle,
		Count
	};

private:
	struct PlaceItem{
		PlaceItemCategory category;
		std::string name;
		D3D12_GPU_DESCRIPTOR_HANDLE texture;
		Vector2 iconSize {64.0f, 64.0f};
		std::function<void()> createFunc; //< 直接作成コマンドを呼ぶ方式に変更
	};

public:
	PlaceToolPanel();
	~PlaceToolPanel() override = default;

	void Render() override;
	void OnSceneContextChanged(SceneContext* newContext) override;

	const std::string& GetPanelName() const override{ return panelName_; }

private:
	void RegisterPlaceItems();
	void RenderCategoryItems();

	SceneContext* pSceneContext_ = nullptr;
	std::unordered_map<PlaceItemCategory, std::vector<PlaceItem>> categoryItems_;
	std::string panelName_ = "PlaceToolPanel";
};
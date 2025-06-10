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

class SceneContext;
class SceneObject;

class PlaceToolPanel
	:public IEngineUI, public ISceneContextListener {
public:
	//===================================================================*/
	//						enums
	//===================================================================*/
	enum class ShapeObjType {
		Plane,
		Cube,
		Sphere,
		Cylinder,
		Cone,
		Torus,
		Count
	};

private:
	//===================================================================*/
	//						structs
	//===================================================================*/
	struct Icon {
		D3D12_GPU_DESCRIPTOR_HANDLE texture;
		Vector2 size{ 64.0f, 64.0f };
	};

public:
	//===================================================================*/
	//						public functions
	//===================================================================*/
	PlaceToolPanel();
	~PlaceToolPanel() override = default;

	void Render() override;
	void OnSceneContextChanged(SceneContext* newContext) override;

	//--------- accessor ------------------------------------------------//
	const std::string& GetPanelName() const override { return panelName_; }

private:
	//===================================================================*/
	//						private functions
	//===================================================================*/
	void CreateShapeObject(ShapeObjType shapeType);

	void RenderShapeObjectButtons();

private:
	//===================================================================*/
	//						private variables
	//===================================================================*/
	SceneContext* pSceneContext_ = nullptr;			//< context
	std::unordered_map<ShapeObjType, Icon> icons_;	//< アイコンのマップ
};


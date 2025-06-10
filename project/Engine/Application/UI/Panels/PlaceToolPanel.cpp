#include "PlaceToolPanel.h"
/* ========================================================================
/*  include Space
/* ===================================================================== */

// engine
#include <Engine/Assets/Texture/TextureManager.h>
#include <Engine/Objects/3D/Actor/BaseGameObject.h>
#include <Engine/Objects/3D/Actor/Library/SceneObjectLibrary.h>
#include <Engine/Objects/3D/Actor/SceneObject.h>
#include <Engine/Scene/Context/SceneContext.h>
#include <Engine/Scene/Utirity/SceneUtility.h>

// externals
#include <externals/imgui/imgui.h>


namespace PlaceToolUtil {
	// 図形オブジェクトのタイプを文字列に変換
	std::string ShapeObjTypeToString(PlaceToolPanel::ShapeObjType type) {
		switch (type) {
			case PlaceToolPanel::ShapeObjType::Plane: return "Plane";
			case PlaceToolPanel::ShapeObjType::Cube: return "Cube";
			case PlaceToolPanel::ShapeObjType::Sphere: return "Sphere";
			case PlaceToolPanel::ShapeObjType::Cylinder: return "Cylinder";
			case PlaceToolPanel::ShapeObjType::Cone: return "Cone";
			case PlaceToolPanel::ShapeObjType::Torus: return "Torus";
			default: return "Unknown";
		}
	}
} // namespace


/////////////////////////////////////////////////////////////////////////////////////////
//		コンストラクタ
/////////////////////////////////////////////////////////////////////////////////////////
PlaceToolPanel::PlaceToolPanel()
	:IEngineUI("PlaceToolPanel") {
	icons_[ShapeObjType::Plane] = { TextureManager::GetInstance()->LoadTexture("UI/Tool/plane.png"), {64.0f, 64.0f} };
	icons_[ShapeObjType::Cube] = { TextureManager::GetInstance()->LoadTexture("UI/Tool/cube.png"), {64.0f, 64.0f} };
	icons_[ShapeObjType::Sphere] = { TextureManager::GetInstance()->LoadTexture("UI/Tool/sphere.png"), {64.0f, 64.0f} };
	icons_[ShapeObjType::Cylinder] = { TextureManager::GetInstance()->LoadTexture("UI/Tool/cylinder.png"), {64.0f, 64.0f} };
	icons_[ShapeObjType::Cone] = { TextureManager::GetInstance()->LoadTexture("UI/Tool/cone.png"), {64.0f, 64.0f} };
	icons_[ShapeObjType::Torus] = { TextureManager::GetInstance()->LoadTexture("UI/Tool/torus.png"), {64.0f, 64.0f} };
}

/////////////////////////////////////////////////////////////////////////////////////////
//		描画
/////////////////////////////////////////////////////////////////////////////////////////
void PlaceToolPanel::Render() {

	ImGui::Begin(panelName_.c_str());

	if (!pSceneContext_) {
		ImGui::Text("sceneContext not set");
		ImGui::End();
		return;
	}

	RenderShapeObjectButtons();

	ImGui::End();
}

/////////////////////////////////////////////////////////////////////////////////////////
//		sceneの更新を通知
/////////////////////////////////////////////////////////////////////////////////////////
void PlaceToolPanel::OnSceneContextChanged(SceneContext* newContext) { pSceneContext_ = newContext; }

/////////////////////////////////////////////////////////////////////////////////////////
//		図形モデルを追加
/////////////////////////////////////////////////////////////////////////////////////////
void PlaceToolPanel::CreateShapeObject(ShapeObjType shapeType) {
	//タイプに応じたオブジェクトを生成
	std::unique_ptr<BaseGameObject> object;
	std::string objectName;
	std::string modelName;

	switch (shapeType) {
		case PlaceToolPanel::ShapeObjType::Plane:
			objectName = "Plane";
			modelName = "plane.obj";
			break;
		case PlaceToolPanel::ShapeObjType::Cube:
			objectName = "Cube";
			modelName = "debugCube.obj";
			break;
		case PlaceToolPanel::ShapeObjType::Sphere:
			objectName = "Sphere";
			modelName = "debugSphere.obj";
			break;
		case PlaceToolPanel::ShapeObjType::Cylinder:
			objectName = "Cylinder";
			modelName = "cylinder.obj";
			break;
		case PlaceToolPanel::ShapeObjType::Cone:
			objectName = "Cone";
			modelName = "cone.obj";
			break;
		case PlaceToolPanel::ShapeObjType::Torus:
			objectName = "Torus";
			modelName = "torus.obj";
			break;

		case PlaceToolPanel::ShapeObjType::Count:
			break;
		default:
			break;
	}

	object = std::make_unique<BaseGameObject>(modelName, objectName);
	object->Initialize();
	pSceneContext_->GetMeshRenderer()->Register(object->GetModel(), &object->GetWorldTransform());
	pSceneContext_->AddEditorObject(std::move(object));

}

/////////////////////////////////////////////////////////////////////////////////////////
//		図形モデルを追加
/////////////////////////////////////////////////////////////////////////////////////////
void PlaceToolPanel::RenderShapeObjectButtons() {

	ImGui::Text("Primitives");

	// タイルレイアウトを開始
	for (auto& [type, icon] : icons_) {
		ImGui::PushID(static_cast<int>(type));

		ImGui::BeginGroup();

		ImVec2 iconSize = ImVec2(icon.size.x,icon.size.y);
		// アイコンボタン
		if (ImGui::ImageButton((ImTextureID)icon.texture.ptr, iconSize)) {
			CreateShapeObject(type);
		}
		if (ImGui::IsItemHovered()) {
			ImGui::SetTooltip("%s", PlaceToolUtil::ShapeObjTypeToString(type).c_str());
		}

		// ラベルは画像の右に配置
		ImGui::SameLine();
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + icon.size.y * 0.25f); //< ラベル位置調整
		ImGui::Text("%s", PlaceToolUtil::ShapeObjTypeToString(type).c_str());

		ImGui::EndGroup();
		ImGui::Spacing();

		ImGui::PopID();
	}

}


#include "LevelEditor.h"
// engine
#include <Engine/Application/UI/EngineUI/Context/EditorContext.h>
#include <Engine/Application/Input/Input.h>
#include <Engine/Graphics/Camera/Manager/CameraManager.h>
#include <Engine/Physics/Ray/Raycastor.h>
#include <Engine/Objects/3D/Actor/Library/SceneObjectLibrary.h>
#include <Engine/Scene/Context/SceneContext.h>


void LevelEditor::Initialize() {

	// 各パネルの初期化
	hierarchy_ = std::make_unique<HierarchyPanel>();
	editor_ = std::make_unique<EditorPanel>();
	inspector_ = std::make_unique<InspectorPanel>();
	sceneEditor_ = std::make_unique<SceneObjectEditor>();
	placeToolPanel_ = std::make_unique<PlaceToolPanel>();

	// Panel に LevelEditor 自体を渡す（コールバック通知や setter）
	editor_->SetOnEditorSelected([this](BaseEditor* editor) {
		SetSelectedEditor(editor);
	});
	hierarchy_->SetOnObjectSelected([this] (const std::shared_ptr<SceneObject>& obj){
		SetSelectedObject(obj);
									});
	inspector_->SetSceneObjectEditor(sceneEditor_.get());

	// ビューポートの初期化
	mainViewport_ = std::make_unique<Viewport>(ViewportType::VIEWPORT_MAIN, "Game Viewport");
	mainViewport_->SetCamera(CameraManager::GetInstance()->GetCamera3d());
	debugViewport_ = std::make_unique<Viewport>(ViewportType::VIEWPORT_DEBUG, "Debug Viewport");
	debugViewport_->SetCamera(CameraManager::GetDebugCamera());

	// Manipulator をツールとして登録
	if (auto* manipulator = sceneEditor_->GetManipulator()) {
		debugViewport_->AddTool(manipulator);
	}

}

void LevelEditor::Update() {
#ifdef _DEBUG
		//// 入力チェックはここで行う
	//if (Input::GetInstance()->TriggerMouseButton(0)) {
	//	TryPickUnderCursor(); // レイキャストして選択処理へ
	//}9
#endif // _DEBUG
}

void LevelEditor::Render() {
	hierarchy_->Render();
	editor_->Render();
	placeToolPanel_->Render();

	inspector_->SetSelectedEditor(selectedEditor_);
	inspector_->SetSelectedObject(selectedObject_);
	inspector_->Render();

	sceneEditor_->Update();
}

void LevelEditor::SetSelectedEditor(BaseEditor* editor) {
	selectedEditor_ = editor;
	selectedObject_ = nullptr;
	inspector_->SetSelectedEditor(editor);
}

void LevelEditor::SetSelectedObject(const std::shared_ptr<SceneObject>& object) {
	selectedObject_ = object;
	selectedEditor_ = nullptr;
	inspector_->SetSelectedObject(object);
}

void LevelEditor::RenderViewport(ViewportType type, const ImTextureID& tex) {
	//タイプに応じて描画
	if (type == ViewportType::VIEWPORT_MAIN) {
		if (mainViewport_) {
			mainViewport_->Render(tex);
		}
	} else if (type == ViewportType::VIEWPORT_DEBUG) {
		if (debugViewport_) {
			debugViewport_->Render(tex);
		}
	}
}

void LevelEditor::TryPickObjectFromMouse(const Vector2& mouse, const Vector2& viewportSize, const Matrix4x4& view, const Matrix4x4& proj){
	Ray ray = Raycastor::ConvertMouseToRay(mouse, view, proj, viewportSize);
	auto picked = PickSceneObjectByRay(ray);
	if (picked){
		SetSelectedObject(picked);
	}
}

std::shared_ptr<SceneObject> LevelEditor::PickSceneObjectByRay(const Ray& ray){
	const auto* lib = hierarchy_->GetSceneObjectLibrary();
	if (!lib) return nullptr;

	const auto& allObjects = lib->GetAllObjects(); // std::vector<std::shared_ptr<SceneObject>>
	auto hit = Raycastor::Raycast(ray, allObjects);
	if (hit){
		// RaycastHit 内の hitObject は shared_ptr と一致していないため、明示的に探す必要あり
		for (const auto& obj : allObjects){
			if (obj == hit->hitObject){
				return obj;
			}
		}
	}
	return nullptr;
}

void LevelEditor::NotifySceneContextChanged(SceneContext* newContext){
	placeToolPanel_->OnSceneContextChanged(newContext);
}

void LevelEditor::TryPickUnderCursor(){
	Vector2 origin = debugViewport_->GetPosition();	// ビューポート描画位置（スクリーン座標）
	Vector2 size = debugViewport_->GetSize();		// ビューポートの実際のサイズ（ピクセル）

	ImVec2 mouse = ImGui::GetMousePos();

	float relativeX = mouse.x - origin.x;
	float relativeY = mouse.y - origin.y;

	if (relativeX < 0 || relativeY < 0 || relativeX > size.x || relativeY > size.y) return;

	Vector2 mousePos = Vector2(relativeX, relativeY);

	Matrix4x4 view = CameraManager::GetDebugCamera()->GetViewMatrix();
	Matrix4x4 proj = CameraManager::GetDebugCamera()->GetProjectionMatrix();

	Ray ray = Raycastor::ConvertMouseToRay(mousePos, view, proj, size);
	auto picked = PickSceneObjectByRay(ray);
	if (picked){
		SetSelectedObject(picked);
	}
}
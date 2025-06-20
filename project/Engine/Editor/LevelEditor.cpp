#include "LevelEditor.h"
// engine
#include <Engine/Application/UI/EngineUI/Context/EditorContext.h>
#include <Engine/Application/Input/Input.h>
#include <Engine/Graphics/Camera/Manager/CameraManager.h>
#include <Engine/Physics/Ray/Raycastor.h>
#include <Engine/Objects/3D/Actor/Library/SceneObjectLibrary.h>
#include <Engine/Scene/Context/SceneContext.h>
#include <Engine/Scene/Serializer/SceneSerializer.h>

#include <externals/imgui/ImGuiFileDialog.h>

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
	hierarchy_->SetOnObjectSelected([this](SceneObject* obj) {
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

	// エディターメニューの初期化
	menu_ = std::make_unique<EditorMenu>();
	menu_->Add(MenuCategory::File, {
		"Save Scene", "Ctrl+S", [this]() {
			IGFD::FileDialogConfig config;
			config.path = "Resources/Assets/Scenes/";
			ImGuiFileDialog::Instance()->OpenDialog(
				"SceneSaveDialog",
				"load scene file",
				".json",
				config
			);
		}, true
			   });

	menu_->Add(MenuCategory::File, {
	"Open Scene", "Ctrl+O", [] {
		IGFD::FileDialogConfig config;
		config.path = "Resources/Assets/Scenes/";
		ImGuiFileDialog::Instance()->OpenDialog(
			"SceneOpenDialog",
			"open scene",
			".json",
			config
		);
	}, true
			  });

}

void LevelEditor::Update() {
#ifdef _DEBUG
		// 入力チェックはここで行う
	if (Input::GetInstance()->TriggerMouseButton(MouseButton::Left)) {
		TryPickUnderCursor(); // レイキャストして選択処理へ
	}
#endif // _DEBUG
}

void LevelEditor::Render() {
	hierarchy_->Render();
	editor_->Render();
	placeToolPanel_->Render();
	menu_->Render();

	inspector_->SetSelectedEditor(selectedEditor_);
	inspector_->SetSelectedObject(selectedObject_);

	// ----------------------------
	// Open Scene ダイアログ処理
	// ----------------------------
	if (ImGuiFileDialog::Instance()->Display("SceneOpenDialog")){
		if (ImGuiFileDialog::Instance()->IsOk()){
			std::string filePath = ImGuiFileDialog::Instance()->GetFilePathName();
			ClearSelection(); // 既存の選択をクリア
			SceneSerializer::Load(*pSceneContext_, filePath);
		}
		ImGuiFileDialog::Instance()->Close();
	}

	// ----------------------------
	// Save Scene ダイアログ処理
	// ----------------------------
	if (ImGuiFileDialog::Instance()->Display("SceneSaveDialog")){
		if (ImGuiFileDialog::Instance()->IsOk()){
			std::string filePath = ImGuiFileDialog::Instance()->GetFilePathName();
			SceneSerializer::Save(*pSceneContext_, filePath);
		}
		ImGuiFileDialog::Instance()->Close();
	}

	inspector_->Render();

	sceneEditor_->Update();
}

void LevelEditor::SetSelectedEditor(BaseEditor* editor) {
	selectedEditor_ = editor;
	selectedObject_ = nullptr;
	inspector_->SetSelectedEditor(editor);
}

void LevelEditor::SetSelectedObject(SceneObject* object) {
	selectedObject_ = object;
	selectedEditor_ = nullptr;
	hierarchy_->SetSelectedObject(object);
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

void LevelEditor::TryPickObjectFromMouse(const Vector2& mouse, const Vector2& viewportSize, const Matrix4x4& view, const Matrix4x4& proj) {
	Vector2 mouseLocal = mouse - debugViewport_->GetPosition();
	Ray ray = Raycastor::ConvertMouseToRay(mouseLocal, view, proj, viewportSize);
	if (SceneObject* obj = PickSceneObjectByRay(ray)) {
		SetSelectedObject(obj);
	}
}

SceneObject* LevelEditor::PickSceneObjectByRay(const Ray& ray) {
	const auto* lib = hierarchy_->GetSceneObjectLibrary();
	if (!lib) return nullptr;

	const auto& allObjects = lib->GetAllObjects();
	auto hit = Raycastor::Raycast(ray, allObjects);
	if (hit) {
		return static_cast<SceneObject*>(hit->hitObject);
	}
	return nullptr;
}

void LevelEditor::SaveScene() {
	std::string scenePath = "Resources/Assets/Scenes/" + pSceneContext_->GetSceneName() + ".json";
	SceneSerializer::Save(*pSceneContext_, scenePath);
}

void LevelEditor::NotifySceneContextChanged(SceneContext* newContext){
	// 新しい ObjectLibrary を各パネルに通知
	hierarchy_->SetSceneObjectLibrary(newContext ? newContext->GetObjectLibrary() : nullptr);
	placeToolPanel_->OnSceneContextChanged(newContext);
	pSceneContext_ = newContext;

	// 現在の選択オブジェクトをリセット（古いシーンに属している可能性あり）
	SetSelectedObject(nullptr);
	ClearSelection(); // ⬅️ 明示的に無効化（ダングリング防止）

	if (newContext){
		// HierarchyPanel: 削除されたら選択解除
		newContext->AddOnObjectRemovedListener(
			[editor = this] (SceneObject* removed){
				if (editor->GetHierarchyPanel()->GetSelectedObject() == removed){
					editor->SetSelectedObject(nullptr);
				}
			}
		);

		// SceneObjectEditor: 削除されたら無効化
		sceneEditor_->BindRemovalCallback(newContext);
	}
}
void LevelEditor::TryPickUnderCursor() {
	Vector2 origin = debugViewport_->GetPosition();	// ビューポート描画位置（スクリーン座標）
	Vector2 size = debugViewport_->GetSize();		// ビューポートの実際のサイズ（ピクセル）

	// ImGui上のマウス位置
	ImVec2 mouse = ImGui::GetMousePos();

	// ビューポート内のローカルマウス座標
	float relativeX = mouse.x - origin.x;
	float relativeY = mouse.y - origin.y;

	// 範囲外なら無視
	if (relativeX < 0 || relativeY < 0 || relativeX > size.x || relativeY > size.y) return;

	// スクリーン→ビューポート空間（ピクセル）
	Vector2 mousePos = Vector2(relativeX, relativeY);

	// Ray生成
	Matrix4x4 view = CameraManager::GetDebugCamera()->GetViewMatrix();
	Matrix4x4 proj = CameraManager::GetDebugCamera()->GetProjectionMatrix();

	Ray ray = Raycastor::ConvertMouseToRay(mousePos, view, proj, size);
	if (SceneObject* picked = PickSceneObjectByRay(ray)) {
		SetSelectedObject(picked);
	}
}
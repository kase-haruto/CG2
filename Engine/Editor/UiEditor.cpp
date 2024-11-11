#include "Editor/UiEditor.h"

/* engine */
#include "objects/Sprite.h"
#include "objects/TextureManager.h"

#ifdef _DEBUG
#include "imgui.h"
#endif // _DEBUG

/* lib */
#include <string>

UIEditor::UIEditor(){
	// テクスチャマネージャーのインスタンスを取得
	textureManager_ = TextureManager::GetInstance();
}

void UIEditor::ShowImGuiInterface(){
#ifdef _DEBUG


	ImGui::Begin("Textures");

	// テクスチャリストの表示
	static std::string selectedTexture = "uvChecker.png"; // デフォルトの選択テクスチャ名

	if (ImGui::BeginCombo("select", selectedTexture.c_str())){
		// テクスチャマネージャーからロード済みのテクスチャリストを取得
		for (const auto& texturePair : textureManager_->GetLoadedTextures()){
			bool isSelected = (selectedTexture == texturePair.first);
			if (ImGui::Selectable(texturePair.first.c_str(), isSelected)){
				selectedTexture = texturePair.first;
			}
			if (isSelected){
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}


	// 選択されたテクスチャのプレビュー表示
	D3D12_GPU_DESCRIPTOR_HANDLE srvHandle = textureManager_->GetSrvHandle(selectedTexture);
	if (srvHandle.ptr != 0){
		ImTextureID textureId = reinterpret_cast< ImTextureID >(srvHandle.ptr);
		ImGui::Image(textureId, ImVec2(128, 128)); // プレビューのサイズを設定
	}

	// スプライトの生成用の入力欄
	static Vector2 newPosition = {400.0f, 0.0f}; // デフォルトの位置
	static Vector2 newSize = {64.0f, 64.0f};     // デフォルトのサイズ

	ImGui::SliderFloat2("Position", &newPosition.x, 0.0f, 1280.0f); // 位置の入力
	ImGui::SliderFloat2("Size", &newSize.x, 0.0f, 512.0f);          // サイズの入力

	// スプライトの作成ボタン
	if (ImGui::Button("Create")){
		AddSprite(selectedTexture, newPosition, newSize);
	}

	// スプライトリストの表示
	if (ImGui::CollapsingHeader("Sprites List", ImGuiTreeNodeFlags_DefaultOpen)){
		ImGui::Text("Select a sprite to edit its properties:");

		for (auto& sprite : sprites_){
			// 名前をわかりやすくするため、スプライトのインデックス番号を表示
			// アドレスを利用してのラベルを生成
			std::string spriteLabel = "Sprite " + std::to_string(reinterpret_cast< uintptr_t >(sprite.get()));

			// 各スプライトごとに CollapsingHeader を作成
			if (ImGui::CollapsingHeader(spriteLabel.c_str(), ImGuiTreeNodeFlags_None)){
				// スプライトのプロパティ編集
				Vector2 position = sprite->GetPosition();
				float rotate = sprite->GetRotation();
				Vector2 size = sprite->GetSize();

				// アドレスを使ったのIDを生成
				std::string uniqueId = std::to_string(reinterpret_cast< uintptr_t >(sprite.get()));

				// 位置の調整
				ImGui::SliderFloat2(("Position##" + uniqueId).c_str(), &position.x, 0.0f, 1280.0f);
				sprite->SetPosition(position);

				// 回転の調整
				ImGui::SliderAngle(("Rotation##" + uniqueId).c_str(), &rotate, -180.0f, 180.0f);
				sprite->SetRotation(rotate);

				// サイズの調整
				ImGui::SliderFloat2(("Size##" + uniqueId).c_str(), &size.x, 1.0f, 512.0f);
				sprite->SetSize(size);
			}
		}

	}


	ImGui::End();
#endif // _DEBUG
}

void UIEditor::AddSprite(const std::string& textureName, const Vector2& position, const Vector2& size){
	// 指定されたテクスチャ名でテクスチャをロード
	D3D12_GPU_DESCRIPTOR_HANDLE textureHandle = textureManager_->LoadTexture(textureName);
	auto sprite = std::make_shared<Sprite>(textureName);

	// スプライトの初期化
	sprite->Initialize(position, size);
	sprite->SetTextureHandle(textureHandle);

	// スプライトリストに追加
	sprites_.emplace_back(sprite);
}

void UIEditor::Update(){
	for (auto& sprite: sprites_){
		sprite->Update();
	}
}

void UIEditor::Draw(){
	for (const auto& sprite : sprites_){
		sprite->Draw();
	}
}
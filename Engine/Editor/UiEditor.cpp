#include "Editor/UiEditor.h"

/* engine */
#include "objects/Sprite.h"
#include "objects/TextureManager.h"

#ifdef _DEBUG
#include "imgui.h"
#endif // _DEBUG

/* lib */
#include "string"

UIEditor::UIEditor(){
	// テクスチャマネージャーのインスタンスを取得
	textureManager_ = TextureManager::GetInstance();
}

void UIEditor::ShowImGuiInterface(){
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

	// スプライトの作成ボタン
	if (ImGui::Button("Create")){
		AddSprite(selectedTexture);
	}

	// スプライトリストの表示
	if (ImGui::CollapsingHeader("sprite")){
		for (size_t i = 0; i < sprites_.size(); ++i){
			if (ImGui::Selectable(("sprite " + std::to_string(i)).c_str(), selectedSpriteIndex_ == i)){
				selectedSpriteIndex_ = static_cast< int >(i);
			}
		}
	}

	// 選択されたスプライトのプロパティを表示
	if (selectedSpriteIndex_ >= 0 && selectedSpriteIndex_ < sprites_.size()){
		ImGui::Separator();

		auto& sprite = sprites_[selectedSpriteIndex_];

		Vector2 position = sprite->GetPosition();
		float rotate = sprite->GetRotation();
		Vector2 size = sprite->GetSize();

		ImGui::SliderFloat2("position", &position.x, 0.0f, 1280.0f);
		ImGui::DragFloat("rotate", &rotate, 0.01f);
		ImGui::DragFloat3("scale", &size.x, 0.01f);

		sprite->SetPosition(position);
		sprite->SetRotation(rotate);
		sprite->SetSize(size);
	}

	ImGui::End();
}

void UIEditor::AddSprite(const std::string& textureName){
	// 指定されたテクスチャ名でテクスチャをロード
	D3D12_GPU_DESCRIPTOR_HANDLE textureHandle = textureManager_->LoadTexture(textureName);
	auto sprite = std::make_shared<Sprite>(textureName);

	// スプライトの初期化
	sprite->Initialize({400.0f,0.0f} ,{64.0f,64.0f});
	sprite->SetTextureHandle(textureHandle);

	// スプライトリストに追加
	sprites_.emplace_back(sprite);
}

void UIEditor::Update(){
	// 現在選択されているスプライトを更新
	if (selectedSpriteIndex_ >= 0 && selectedSpriteIndex_ < sprites_.size()){
		sprites_[selectedSpriteIndex_]->Update();
	}
}

void UIEditor::Draw(){
	for (const auto& sprite : sprites_){
		sprite->Draw();
	}
}
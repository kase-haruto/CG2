#include "SkyBox.h"

#include <lib/myMath/Matrix4x4.h>
#include <Engine/graphics/GraphicsGroup.h>
#include <Engine/graphics/camera/CameraManager.h>
#include <Engine/graphics/camera/BaseCamera.h>
#include <Engine/objects/TextureManager.h>

SkyBox::SkyBox(std::string fileName, std::function<void(IMeshRenderable*)>cb) {
	cb(this);
	SceneObject::SetName("skyBox", ObjectType::GameObject);
	SceneObject::EnableGuiList();

	textureName_ = fileName;
}

void SkyBox::Initialize() {
	auto device = GraphicsGroup::GetInstance()->GetDevice();
	vertexBuffer_.Initialize(device, static_cast<UINT>(vertices_.size()));
	indexBuffer_.Initialize(device, static_cast<UINT>(indices_.size()));
	worldTransform_.Initialize();
}

void SkyBox::ShowGui() {
	worldTransform_.ShowImGui();
}

void SkyBox::Update(const Vector3& center, const Vector3& size) {
	Vector3 half = size * 0.5f;

	// --- 頂点データ設定 ---
	vertices_[0].position = { +1.0f, +1.0f, +1.0f, 1.0f }; // 右
	vertices_[1].position = { +1.0f, +1.0f, -1.0f, 1.0f };
	vertices_[2].position = { +1.0f, -1.0f, +1.0f, 1.0f };
	vertices_[3].position = { +1.0f, -1.0f, -1.0f, 1.0f };

	vertices_[4].position = { -1.0f, +1.0f, -1.0f, 1.0f }; // 左
	vertices_[5].position = { -1.0f, +1.0f, +1.0f, 1.0f };
	vertices_[6].position = { -1.0f, -1.0f, -1.0f, 1.0f };
	vertices_[7].position = { -1.0f, -1.0f, +1.0f, 1.0f };

	vertices_[8].position =  { -1.0f, +1.0f, +1.0f, 1.0f }; // 前
	vertices_[9].position =  { +1.0f, +1.0f, +1.0f, 1.0f };
	vertices_[10].position = { -1.0f, -1.0f, +1.0f, 1.0f };
	vertices_[11].position = { +1.0f, -1.0f, +1.0f, 1.0f };

	vertices_[12].position = { +1.0f, -1.0f, -1.0f, 1.0f }; // 後
	vertices_[13].position = { -1.0f, -1.0f, -1.0f, 1.0f };
	vertices_[14].position = { +1.0f, +1.0f, -1.0f, 1.0f };
	vertices_[15].position = { -1.0f, +1.0f, -1.0f, 1.0f };

	vertices_[16].position = { -1.0f, +1.0f, -1.0f, 1.0f }; // 上
	vertices_[17].position = { +1.0f, +1.0f, -1.0f, 1.0f };
	vertices_[18].position = { -1.0f, +1.0f, +1.0f, 1.0f };
	vertices_[19].position = { +1.0f, +1.0f, +1.0f, 1.0f };

	vertices_[20].position = { +1.0f, -1.0f, +1.0f, 1.0f }; // 下
	vertices_[21].position = { -1.0f, -1.0f, +1.0f, 1.0f };
	vertices_[22].position = { +1.0f, -1.0f, -1.0f, 1.0f };
	vertices_[23].position = { -1.0f, -1.0f, -1.0f, 1.0f };

	for (int i = 0; i < 24; ++i) {
		Vector3 local = {
			vertices_[i].position.x,
			vertices_[i].position.y,
			vertices_[i].position.z
		};
		Vector3 world = local * half + center;
		vertices_[i].position = { world.x, world.y, world.z, 1.0f };
	/*	vertices_[i].texcoord = { 0.0f, 0.0f };
		vertices_[i].normal = { 0.0f, 0.0f, 0.0f };*/
	}

	uint16_t idx[] = {
		0, 1, 2, 2, 1, 3,       // 右面
		4, 5, 6, 6, 5, 7,       // 左面
		8,9, 10, 10, 9,11,       // 前面
	   12,13,14,14,13,15,       // 後面
	   16,17,18,18,17,19,       // 上面
	   20,21,22,22,21,23        // 下面
	};
	std::copy(std::begin(idx), std::end(idx), indices_.begin());

	// バッファ転送
	vertexBuffer_.TransferData(vertices_.data(), static_cast<UINT>(vertices_.size()));
	indexBuffer_.TransferData(indices_.data(), static_cast<UINT>(indices_.size()));

	worldTransform_.scale = size;
	worldTransform_.translation = center;
	worldTransform_.Update();
}


void SkyBox::Draw() {
	ID3D12GraphicsCommandList* cmd = GraphicsGroup::GetInstance()->GetCommandList().Get();
	CameraManager::SetCommand(cmd, PipelineType::Skybox);

	D3D12_GPU_DESCRIPTOR_HANDLE handle = TextureManager::GetInstance()->LoadTexture(textureName_);

	GraphicsGroup::GetInstance()->SetCommand(cmd, PipelineType::Skybox, BlendMode::NORMAL);
	cmd->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	vertexBuffer_.SetCommand(cmd);
	indexBuffer_.SetCommand(cmd);
	worldTransform_.SetCommand(cmd, 0);
	cmd->SetGraphicsRootDescriptorTable(2, handle);

	cmd->DrawIndexedInstanced(
		static_cast<UINT>(indices_.size()), // IndexCountPerInstance
		1,                                   // InstanceCount
		0,                                   // StartIndexLocation
		0,                                   // BaseVertexLocation
		0                                    // StartInstanceLocation
	);
}

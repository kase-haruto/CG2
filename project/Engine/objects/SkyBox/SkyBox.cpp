#include "SkyBox.h"

#include <lib/myMath/Matrix4x4.h>
#include <Engine/graphics/GraphicsGroup.h>
#include <Engine/graphics/camera/CameraManager.h>
#include <Engine/graphics/camera/BaseCamera.h>
#include <Engine/objects/TextureManager.h>

void SkyBox::Initialize() {
	auto device = GraphicsGroup::GetInstance()->GetDevice();
	vertexBuffer_.Initialize(device, static_cast<UINT>(vertices_.size()));
	indexBuffer_.Initialize(device, static_cast<UINT>(indices_.size()));
	worldTransform_.Initialize();
}

void SkyBox::Update(const Vector3& center, const Vector3& size) {
	Vector3 half = size * 0.5f;

	Vector3 v[8] = {
		center + Vector3{-half.x, -half.y, -half.z}, // 0
		center + Vector3{+half.x, -half.y, -half.z}, // 1
		center + Vector3{+half.x, +half.y, -half.z}, // 2
		center + Vector3{-half.x, +half.y, -half.z}, // 3
		center + Vector3{-half.x, -half.y, +half.z}, // 4
		center + Vector3{+half.x, -half.y, +half.z}, // 5
		center + Vector3{+half.x, +half.y, +half.z}, // 6
		center + Vector3{-half.x, +half.y, +half.z}  // 7
	};

	for (int i = 0; i < 8; ++i) {
		vertices_[i].position = { v[i].x, v[i].y, v[i].z, 1.0f };
		vertices_[i].texcoord = { 0.0f, 0.0f };
		vertices_[i].normal = v[i].Normalize();
	}
	vertexBuffer_.TransferData(vertices_.data(), static_cast<UINT>(vertices_.size()));

	// インデックス（内向き面）
	indices_ = {
		2,1,0,  3,2,0, // -Z
		4,5,6,  4,6,7, // +Z
		1,5,4,  1,4,0, // -Y
		3,7,6,  3,6,2, // +Y
		0,4,7,  0,7,3, // -X
		2,6,5,  2,5,1  // +X
	};
	indexBuffer_.TransferData(indices_.data(), static_cast<UINT>(indices_.size()));

	// WorldTransform セットアップ
	worldTransform_.scale = size;
	worldTransform_.translation = center;
	worldTransform_.Update();
}

void SkyBox::Draw() {
	ID3D12GraphicsCommandList* cmd = GraphicsGroup::GetInstance()->GetCommandList().Get();
	CameraManager::SetCommand(cmd, PipelineType::Skybox);

	D3D12_GPU_DESCRIPTOR_HANDLE handle = TextureManager::GetInstance()->LoadTexture("rostock_laage_airport_4k.dds");

	GraphicsGroup::GetInstance()->SetCommand(cmd, PipelineType::Skybox, BlendMode::NORMAL);
	cmd->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	vertexBuffer_.SetCommand(cmd);
	indexBuffer_.SetCommand(cmd);
	worldTransform_.SetCommand(cmd, 0);
	cmd->SetGraphicsRootDescriptorTable(2, handle);

	cmd->DrawIndexedInstanced(static_cast<UINT>(indices_.size()), 1, 0, 0, 0);
}


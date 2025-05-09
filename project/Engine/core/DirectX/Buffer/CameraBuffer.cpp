#include "CameraBuffer.h"

/* ========================================================================
/*		include space
/* ===================================================================== */

/////////////////////////////////////////////////////////////////////////////////////////
//		初期化処理
/////////////////////////////////////////////////////////////////////////////////////////
void Camera3DBuffer::Initialize(ID3D12Device* device){
	buffer_.Initialize(device);
}

/////////////////////////////////////////////////////////////////////////////////////////
//		更新処理
/////////////////////////////////////////////////////////////////////////////////////////
void Camera3DBuffer::Update(const Matrix4x4& view, const Matrix4x4& proj, const Vector3& worldPos){
	data_.view = view;
	data_.projection = proj;
	data_.viewProjection = Matrix4x4::Multiply(view, proj);
	data_.worldPosition = worldPos;
	// バッファにデータを転送
	buffer_.TransferData(data_);
}

/////////////////////////////////////////////////////////////////////////////////////////
//		コマンドを積む
/////////////////////////////////////////////////////////////////////////////////////////
void Camera3DBuffer::SetCommand(ID3D12GraphicsCommandList* cmdList, PipelineType pipelineType) {
	uint32_t rootParameterIndex = 0;

	if (pipelineType == PipelineType::Object3D || pipelineType == PipelineType::SkinningObject3D) {
		rootParameterIndex = 5;
	} else if (pipelineType == PipelineType::Line) {
		rootParameterIndex = 1;
	}

	buffer_.SetCommand(cmdList, rootParameterIndex);
}

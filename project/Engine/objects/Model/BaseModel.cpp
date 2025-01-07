#include "BaseModel.h"

#include "lib/myFunc/MyFunc.h"

const std::string BaseModel::directoryPath_ = "Resource/models";

Matrix4x4 BaseModel::GetWorldRotationMatrix(){
	// 現在のオブジェクトのローカル回転行列を取得
	Matrix4x4 localRot = EulerToMatrix(transform.rotate);

	// 親が存在する場合、親のワールド回転行列と合成する
	if (parent_ != nullptr){
		Matrix4x4 parentWorldRot = EulerToMatrix(parent_->translate);
		return Matrix4x4::Multiply(parentWorldRot, localRot);
	} else{
		return localRot;
	}
}

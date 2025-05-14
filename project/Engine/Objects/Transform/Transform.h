#pragma once
#include <Engine/Foundation/Math/Vector3.h>
#include <Engine/Foundation/Math/Vector2.h>
#include <Engine/Foundation/Math/Quaternion.h>

#include <Engine/core/DirectX/Buffer/DxConstantBuffer.h>

// c++
#include <string>

struct TransformationMatrix{
	Matrix4x4 world;
	Matrix4x4 WorldInverseTranspose;
	Matrix4x4 WVP;
};

struct EulerTransform{
	Vector3 scale;
	Vector3 rotate;
	Vector3 translate;

	void Initialize(){
		scale = {1.0f,1.0f,1.0f};
		rotate = {0.0f,0.0f,0.0f};
		translate = {0.0f,0.0f,0.0f};
	}

	void ShowImGui(const std::string& lavel = "Transform");
};

struct Transform2D{
	Vector2 scale;
	float rotate;
	Vector2 translate;
	void Initialize(){
		scale = {1.0f,1.0f};
		rotate =0.0f;
		translate = {0.0f,0.0f};
	}
	void ShowImGui(const std::string& lavel = "Transform");
};

struct QuaternionTransform{
	Vector3 scale;
	Quaternion rotate;
	Vector3 translate;
};

//============================================================================*/
//	BaseTransform class
//============================================================================*/
class BaseTransform :
	public DxConstantBuffer<TransformationMatrix>{
public:
	//========================================================================*/
	//	public functions
	//========================================================================*/
	BaseTransform() = default;
	virtual ~BaseTransform() = default;

	//--------- main -----------------------------------------------------
	virtual void Initialize();
	virtual void Update([[maybe_unused]]const Matrix4x4& viewProMatrix){}
	virtual void SetCommand(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList,
							UINT rootParameterIndex){
		DxBuffer::SetCommand(commandList, rootParameterIndex);
	};

	//--------- ImGui ---------------------------------------------------
	virtual void ShowImGui(const std::string& lavel = "Transform");

	//--------- accessor -------------------------------------------------
	virtual Vector3 GetWorldPosition()const;

public:
	//========================================================================*/
	//	public variables
	//========================================================================*/
	Vector3 scale;
	Quaternion rotation;
	Vector3 translation;

	Vector3 eulerRotation;

	TransformationMatrix matrix;
	const BaseTransform* parent = nullptr;
};

//============================================================================*/
//	worldTransform class
//============================================================================*/
class WorldTransform :
	public BaseTransform{
public:
	//========================================================================*/
	//	public functions
	//========================================================================*/
	WorldTransform() = default;
	~WorldTransform()override = default;

	virtual void Update(const Matrix4x4& viewProMatrix) override;
	void Update();
};
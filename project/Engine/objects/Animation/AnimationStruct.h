#pragma once
/* ========================================================================
/*	include space
/* ===================================================================== */
// c++
#include <Engine/objects/Transform.h>
#include <Engine/Graphics/Renderer/Primitive/PrimitiveDrawer.h>

// lib
#include <lib/myMath/Vector3.h>
#include <lib/myMath/Quaternion.h>

// C++
#include <vector>
#include <map>
#include <string>
#include <cstdint>
#include <optional>
#include <span>
#include <array>

/////////////////////////////////////////////////////////////////////////////////////////
//          keyframe
/////////////////////////////////////////////////////////////////////////////////////////
template <typename tValue>
struct Keyframe{
	float time;     // アニメーション時間(秒)
	tValue value;   // 補間対象の値 (Vector3 or Quaternion)
};
using KeyframeQuaternion = Keyframe<Quaternion>;
using KeyframeVector3 = Keyframe<Vector3>;

/////////////////////////////////////////////////////////////////////////////////////////
//          AnimationCurve
/////////////////////////////////////////////////////////////////////////////////////////
template<typename tValue>
struct AnimationCurve{
	std::vector<Keyframe<tValue>> keyframes;
};

/////////////////////////////////////////////////////////////////////////////////////////
//          NodeAnimation
/////////////////////////////////////////////////////////////////////////////////////////
struct NodeAnimation{
	AnimationCurve<Vector3>     translate;  // 平行移動
	AnimationCurve<Quaternion>  rotate;     // 回転
	AnimationCurve<Vector3>     scale;      // スケーリング
};

struct Node{
	QuaternionTransform transform;
	Matrix4x4 localMatrix;
	std::string name;
	std::vector<Node> children;
};

/////////////////////////////////////////////////////////////////////////////////////////
//          Animation
/////////////////////////////////////////////////////////////////////////////////////////
struct Animation{
	float duration = 0.0f;  // アニメーション全体の尺(秒)
	// ※本来は TicksPerSecond など、時間単位の情報もあると便利
	std::map<std::string, NodeAnimation> nodeAnimations;
};

/////////////////////////////////////////////////////////////////////////////////////////
//          skelton
/////////////////////////////////////////////////////////////////////////////////////////
struct Joint{
	QuaternionTransform transform;	//< transform情報
	Matrix4x4 localMatrix;			//< ローカル行列
	Matrix4x4 skeletonSpaceMatrix;	//< スケルトン空間行列
	std::string name;				//< ボーン名
	std::vector<int32_t> children;	//< 子ボーンのインデックス
	int32_t index;					//< インデックス
	std::optional<int32_t> parent;	//< 親ボーンのインデックス
};

struct Skeleton{
	int32_t root;
	std::map<std::string, int32_t> jointMap;
	std::vector<Joint> joints;

	void JointDraw(const Matrix4x4& m){
		Vector3 jointCube[8] = {
			Vector3(0.075f, 0.075f, 0.075f),
			Vector3(-0.075f, 0.075f, 0.075f),
			Vector3(-0.075f, 0.075f,-0.075f),
			Vector3(0.075f, 0.075f,-0.075f),

			Vector3(0.075f,-0.075f, 0.075f),
			Vector3(-0.075f,-0.075f, 0.075f),
			Vector3(-0.075f,-0.075f,-0.075f),
			Vector3(0.075f,-0.075f,-0.075f),
		};

		for (int i = 0; i < 8; i++){
			jointCube[i] = Vector3::Transform(jointCube[i], m);
		}

		int p1 = 0;
		int p2 = 1;
		for (int i = 0; i < 4; i++){
			PrimitiveDrawer::GetInstance()->DrawLine3d(jointCube[p1], jointCube[p2], {1.0f,1.0f,1.0f,1.0f});

			p1++;
			p2++;
			p1 = int(fmod(p1, 4));
			p2 = int(fmod(p2, 4));
		}
		p1 = 4;
		p2 = 5;
		for (int i = 0; i < 4; i++){
			PrimitiveDrawer::GetInstance()->DrawLine3d(jointCube[p1], jointCube[p2], {1.0f,1.0f,1.0f,1.0f});

			p1++;
			p2++;
			p1 = 4 + int(fmod(p1, 4));
			p2 = 4 + int(fmod(p2, 4));
		}
		p1 = 0;
		p2 = 4;
		for (int i = 0; i < 4; i++){
			PrimitiveDrawer::GetInstance()->DrawLine3d(jointCube[p1], jointCube[p2], {1.0f,1.0f,1.0f,1.0f});

			p1++;
			p2++;
		}
	}

	void Draw(){
		for (Joint& joint : joints){
			Vector3 jointPos = {joint.skeletonSpaceMatrix.m[3][0],joint.skeletonSpaceMatrix.m[3][1],joint.skeletonSpaceMatrix.m[3][2]};
			JointDraw(joint.skeletonSpaceMatrix);
			if (joint.parent){
				Vector3 parentPos = {joints[*joint.parent].skeletonSpaceMatrix.m[3][0],joints[*joint.parent].skeletonSpaceMatrix.m[3][1] ,joints[*joint.parent].skeletonSpaceMatrix.m[3][2]};
				PrimitiveDrawer::GetInstance()->DrawLine3d(jointPos, parentPos, {1.0f,1.0f,1.0f,1.0f});
			}
		}
	}
};

/////////////////////////////////////////////////////////////////////////////////////////
//          skinning
/////////////////////////////////////////////////////////////////////////////////////////
struct VertexWeightData{
	float weight;
	uint32_t vertexIndex;
};

struct JointWeightData{
	Matrix4x4 inverseBindPoseMatrix;
	std::vector<VertexWeightData> vertexWeights;
};

/* ===============================================================
  influence
 =================================================================
頂点はweightの割合だけjointの影響を受ける。
頂点に対して影響を与える(受ける)パラメータ群のこと
============================================================== */
const uint32_t kNumMaxInfluence = 4; // 最大のジョイント影響数
struct VertexInfluence{
	std::array<float, kNumMaxInfluence> weights; // ウェイト
	std::array<int32_t, kNumMaxInfluence> jointIndices; // ジョイントインデックス
};

struct WellForGPU{
	Matrix4x4 skeletonSpaceMatrix;                  //位置用
	Matrix4x4 skeletonSpaceInverseTransposeMatrix;  //法線用
};

struct SkinCluster{
	// jointのinverseBindPoseMatrixがindex順に保存されている
	std::vector<Matrix4x4> inverseBindPoseMatrices;

	Microsoft::WRL::ComPtr<ID3D12Resource>influenceResource;
	D3D12_VERTEX_BUFFER_VIEW influenceBufferView;
	std::span<VertexInfluence>mappedInfluence;

	/* ===============================================================
		matrixPalette
	 =================================================================
	skinningを行う際に必要な行列をskeletonのjointの数だけ格納した配列のこと。
	paletteの色を置く１単位の場所をwellと呼ぶ。
	indexアクセスを必要とするためstructuedBufferを使用する。
	============================================================== */
	Microsoft::WRL::ComPtr<ID3D12Resource>paletteResource;
	std::span<WellForGPU>mappedPalette;
	std::pair<D3D12_CPU_DESCRIPTOR_HANDLE, D3D12_GPU_DESCRIPTOR_HANDLE> paletteSrvHandle;
};


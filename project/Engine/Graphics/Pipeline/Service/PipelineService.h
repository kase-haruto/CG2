#pragma once

#include <Engine/Graphics/Pipeline/Shader/ShaderCompiler.h>
#include <Engine/Graphics/Pipeline/Factory/PsoFactory.h>
#include <Engine/Graphics/Pipeline/Library/PsoLibrary.h>
#include <Engine/Graphics/Pipeline/Pso/PsoDetails.h>

#include <array>

class PipelineService {
private:
	//===================================================================*/
	//		structs
	//===================================================================*/
	struct PipelineKey{
		PipelineTag::Object tag;
		BlendMode blend;

		bool operator==(const PipelineKey& rhs) const{
			return tag == rhs.tag && blend == rhs.blend;
		}
	};

	struct PipelineKeyHasher{
		std::size_t operator()(const PipelineKey& k) const{
			return std::hash<int>()(static_cast< int >(k.tag))
				^ (std::hash<int>()(static_cast< int >(k.blend)) << 1);
		}
	};

public:
	//===================================================================*/
	//		public functions
	//===================================================================*/
	PipelineService();
	~PipelineService() = default;

	void RegisterAllPipelines();

	void Register(const GraphicsPipelineDesc& desc);
	void SetCommand(const GraphicsPipelineDesc& desc, ID3D12GraphicsCommandList* cmd){
		GetPipelineSet(desc).SetCommand(cmd);
	}

	//--------- accessor -------------------------------------------------//
	const PipelineSet GetPipelineSet(const GraphicsPipelineDesc& desc)const {
		return {library_->GetOrCreate(desc),library_->GetRoot(desc)};
	}
	ID3D12PipelineState* GetPipelineState(const GraphicsPipelineDesc& desc) { return library_->GetOrCreate(desc); }
	ID3D12RootSignature* GetRootSig(const GraphicsPipelineDesc& desc) { return library_->GetRoot(desc); }

	PipelineSet GetPipelineSet(PipelineTag::Object tag, BlendMode blend = BlendMode::NORMAL) const;
	PipelineSet GetPipelineSet(PipelineTag::PostProcess tag) const;

	ShaderCompiler* GetCompiler()  const { return compiler_.get(); }
	PsoFactory* GetFactory()  const { return factory_.get(); }
	PsoLibrary* GetLibrary()  const { return library_.get(); }

private:
	//===================================================================*/
	//		private variables
	//===================================================================*/
	std::unique_ptr<ShaderCompiler>   compiler_;
	std::unique_ptr<PsoFactory>  factory_;
	std::unique_ptr<PsoLibrary>  library_;

	std::unordered_map<PipelineKey, PipelineSet, PipelineKeyHasher> objCache_;
	std::array<PipelineSet, static_cast< size_t >(PipelineTag::PostProcess::Count)> ppCache_ {};

};

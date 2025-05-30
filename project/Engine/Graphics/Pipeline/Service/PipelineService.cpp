#include "PipelineService.h"

#include <Engine/Graphics/Pipeline/Presets/PipelinePresets.h>

/////////////////////////////////////////////////////////////////////////////////////////
//		コンストラクタ
/////////////////////////////////////////////////////////////////////////////////////////
PipelineService::PipelineService() {
	compiler_ = std::make_unique<ShaderCompiler>();
	compiler_->InitializeDXC();
	factory_ = std::make_unique<PsoFactory>(compiler_.get());
	library_ = std::make_unique<PsoLibrary>(factory_.get());
}

/////////////////////////////////////////////////////////////////////////////////////////
//		すべてのパイプラインを登録
/////////////////////////////////////////////////////////////////////////////////////////
void PipelineService::RegisterAllPipelines() {

	//===================================================================*/
	//					パイプラインの作成、登録
	//===================================================================*/

	auto regObj = [&] (PipelineTag::Object tag, BlendMode mode, auto makeFn){
		// desc 作成
		GraphicsPipelineDesc desc = makeFn(mode);
		// PSO / RootSignature の取得
		auto pso = library_->GetOrCreate(desc);
		auto root = library_->GetRoot(desc);
		// キャッシュに登録
		PipelineKey key {tag, mode};
		objCache_[key] = {pso, root};
		};

	auto regPP = [&] (PipelineTag::PostProcess tag, auto makeFn){
		GraphicsPipelineDesc desc = makeFn();
		auto pso = library_->GetOrCreate(desc);
		auto root = library_->GetRoot(desc);
		ppCache_[static_cast< size_t >(tag)] = {pso, root};
		};


	//=================== Object Pipelines ================================

	//===================================================================*/
	//						Object3D Pipelines
	//===================================================================*/
	for (int i = 0; i < static_cast< int >(BlendMode::kBlendModeCount); ++i){
		BlendMode mode = static_cast< BlendMode >(i);
		regObj(PipelineTag::Object::Object3d, mode, PipelinePresets::MakeObject3D);
	}

	//===================================================================*/
	//						SkinObject3D Pipelines
	//===================================================================*/
	for (int i = 0; i < static_cast< int >(BlendMode::kBlendModeCount); ++i){
		BlendMode mode = static_cast< BlendMode >(i);
		regObj(PipelineTag::Object::SkinningObject3D, mode, PipelinePresets::MakeSkinningObject3D);
	}

	//=================== PostProcess Pipelines ==========================
	regPP(PipelineTag::PostProcess::GrayScale, PipelinePresets::MakeGrayScale);
	regPP(PipelineTag::PostProcess::RadialBlur, PipelinePresets::MakeRadialBlur);
	regPP(PipelineTag::PostProcess::ChromaticAberration, PipelinePresets::MakeChromaticAberration);
	regPP(PipelineTag::PostProcess::CopyImage, PipelinePresets::MakeCopyImage);

}

/////////////////////////////////////////////////////////////////////////////////////////
//		登録
/////////////////////////////////////////////////////////////////////////////////////////
void PipelineService::Register(const GraphicsPipelineDesc& desc) {
	library_->GetOrCreate(desc);
}

/////////////////////////////////////////////////////////////////////////////////////////
//		pso取得(グラフィックパイプライン)
/////////////////////////////////////////////////////////////////////////////////////////
PipelineSet PipelineService::GetPipelineSet(PipelineTag::Object tag, BlendMode blend) const{
	PipelineKey key {tag, blend};
	auto it = objCache_.find(key);
	if (it != objCache_.end()){
		return it->second;
	}
	assert(false && "Pipeline not found!");
	return {};
}


/////////////////////////////////////////////////////////////////////////////////////////
//		pso取得(ポストプロセス
/////////////////////////////////////////////////////////////////////////////////////////
PipelineSet PipelineService::GetPipelineSet(PipelineTag::PostProcess tag) const{
	return ppCache_[static_cast< size_t >(tag)];
}
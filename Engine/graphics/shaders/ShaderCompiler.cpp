#include "ShaderCompiler.h"
#include"myFUnc/ConvertString.h"
#include<format>


void ShaderCompiler::InitializeDXC() {
	HRESULT hr = S_FALSE;

	//dxcCompilerを初期化
	hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils));
	assert(SUCCEEDED(hr));
	hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler));
	assert(SUCCEEDED(hr));

	//現時点でincludeはしないが、includeに対応するための設定を行っておく
	hr = dxcUtils->CreateDefaultIncludeHandler(&includeHandle);
	assert(SUCCEEDED(hr));
}

void ShaderCompiler::LoadHLSL(const std::wstring& filePath, const wchar_t* profile) {
	//これからシェーダをコンパイルする旨をログに出す
	Log(ConvertString(std::format(L"Begin CompileShader,path:{},profile:{}\n", filePath, profile)));
	//hlslファイルを読む
	
	HRESULT hr = dxcUtils->LoadFile(filePath.c_str(), nullptr, &shaderSource);

	//読めなければ止める
	assert(SUCCEEDED(hr));
	//読み込んだファイルの内容を設定する

	shaderSourceBuffer.Ptr = shaderSource->GetBufferPointer();
	shaderSourceBuffer.Size = shaderSource->GetBufferSize();
	//UTF8の文字コードであることを通知
	shaderSourceBuffer.Encoding = DXC_CP_UTF8;
}

IDxcBlob* ShaderCompiler::CompileShader(
	//CompilerするShaderファイルへのパス
	const std::wstring& filePath,
	//Compilerに使用するProfile
	const wchar_t* profile,
	//初期化で生成したものを3つ
	IDxcUtils* dxcUtils,
	IDxcCompiler3* dxcompiler,
	IDxcIncludeHandler* includeHandler
) {
	//hlslファイルを読む
	LoadHLSL(filePath, profile);
	//Compileする
	Compile(filePath, profile, dxcompiler, includeHandler);
	//警告、エラーが出てないか確認
	CheckNoError();
	//compile結果を受け取って返す
	return GetCompileResult(filePath, profile);
}




void ShaderCompiler::Compile(const std::wstring& filePath,
							 const wchar_t* profile,
							 IDxcCompiler3* dxcompiler,
							 IDxcIncludeHandler* includeHandler) {
	HRESULT hr = S_FALSE;
	LPCWSTR arguments[] = {
		filePath.c_str(),//コンパイル対象のhlslファイル名
		L"-E",L"main",//エントリーポイントの指定。基本的にmain以外には市内
		L"-T",profile,//ShaderProfileの設定
		L"-zi",L"-Qembed_debug",//デバッグ用の情報を埋め込む
		L"-Od",//最適化を外しておく
		L"-Zpr",//メモリレイアウトは行優先
	};

	//実際にshaderをコンパイルする
	hr = dxcCompiler->Compile(
		&shaderSourceBuffer,//読み込んだファイル
		arguments,//コンパイルオプション
		_countof(arguments),//コンパイルオプションの数
		includeHandler,//includeが含まれた諸々
		IID_PPV_ARGS(&shaderResult)//コンパイル結果
	);
	//コンパイルエラーではなくdxcが起動できないなど致命的状況
	assert(SUCCEEDED(hr));
}

void ShaderCompiler::CheckNoError() {
	//警告・エラーが出ていたらログに出して止める
	IDxcBlobUtf8* shaderError = nullptr;
	if (shaderError != nullptr && shaderError->GetStringLength() != 0) {
		Log(shaderError->GetStringPointer());
		//警告。エラーダメ絶対
		assert(false);
	}
}



IDxcBlob* ShaderCompiler::GetCompileResult(const std::wstring& filePath,
										   const wchar_t* profile) {
	HRESULT hr = S_FALSE;
	IDxcBlob* shaderBlob = nullptr;
	//コンパイル結果から実行用のバイナリ部分を取得
	hr = shaderResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderBlob), nullptr);
	assert(SUCCEEDED(hr));
	//成功したログを出す
	Log(ConvertString(std::format(L"Compile Succeded,path:{},profile:{}\n",filePath,profile)));
	//もう使わないリソースを開放
	shaderSource->Release();
	shaderResult->Release();
	//実行用のバイナリ返却
	return shaderBlob;
}



ShaderCompiler::~ShaderCompiler() {
	dxcUtils->Release();
	dxcCompiler->Release();
	includeHandle->Release();
}
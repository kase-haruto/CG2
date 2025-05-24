#include "ShaderCompiler.h"
#include <Engine/Foundation/Utility/ConvertString/ConvertString.h>

// c++
#include <format>

void ShaderCompiler::InitializeDXC() {
	// DXC Compilerを初期化
	HRESULT hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils));
	assert(SUCCEEDED(hr));
	hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler));
	assert(SUCCEEDED(hr));

	// Include handlerを設定
	hr = dxcUtils->CreateDefaultIncludeHandler(&includeHandle);
	assert(SUCCEEDED(hr));
}

void ShaderCompiler::LoadHLSL(const std::wstring& filePath, const wchar_t* profile) {
	//========================================================================
	//	これからシェーダをコンパイルする旨をログに出す
	//========================================================================
	Log(ConvertString(std::format(L"Begin CompileShader, path: {}, profile: {}\n", filePath, profile)));

	//========================================================================
	//	シェーダーファイルのフルパスを構築
	//========================================================================
	std::wstring fullPath = L"Resources/shaders/" + filePath;

	//========================================================================
	//	ファイル読み込み
	//========================================================================
	HRESULT hr = dxcUtils->LoadFile(fullPath.c_str(), nullptr, shaderSource.GetAddressOf());
	assert(SUCCEEDED(hr));
	(void)hr;

	//========================================================================
	//	読み込んだファイル内容を設定
	//========================================================================
	shaderSourceBuffer.Ptr = shaderSource->GetBufferPointer();
	shaderSourceBuffer.Size = shaderSource->GetBufferSize();
	shaderSourceBuffer.Encoding = DXC_CP_UTF8;
}

ComPtr<IDxcBlob> ShaderCompiler::CompileShader(
	const std::wstring& filePath,
	const wchar_t* profile) {
	//========================================================================
	//	HLSLファイルを読み込む
	//========================================================================
	LoadHLSL(filePath, profile);

	//========================================================================
	//	コンパイルする
	//========================================================================
	Compile(filePath, profile);

	//========================================================================
	//	警告・エラーが出てないか確認
	//========================================================================
	CheckNoError();

	//========================================================================
	//	コンパイル結果を返す
	//========================================================================
	return GetCompileResult(filePath, profile);
}

void ShaderCompiler::Compile(const std::wstring& filePath,
							 const wchar_t* profile) {
	HRESULT hr;
	//========================================================================
	//	コンパイルオプションの設定
	//========================================================================
	LPCWSTR arguments[] = {
		filePath.c_str(),         // コンパイル対象
		L"-E", L"main",           // エントリポイント
		L"-T", profile,           // プロファイル
		L"-Zi", L"-Qembed_debug", // デバッグ情報
		L"-Od",                   // 最適化を外す
		L"-Zpr",                  // 行優先
		L"-I", L"Resources/shaders/" // ⭐️ 追加！
	};

	//========================================================================
	//	シェーダをコンパイル
	//========================================================================
	//実際にshaderをコンパイルする
	hr = dxcCompiler->Compile(
		&shaderSourceBuffer,//読み込んだオプション
		arguments,//コンパイルオプション
		_countof(arguments),//コンパイルオプションの数
		includeHandle.Get(),//includeが含まれた諸々
		IID_PPV_ARGS(&shaderResult)//コンパイル結果
	);
	//コンパイルエラーではなくdxcが起動できないなど致命的な状況
	assert(SUCCEEDED(hr));
}

void ShaderCompiler::CheckNoError() {
	ComPtr<IDxcBlobUtf8> shaderError = nullptr;
	HRESULT hr = shaderResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(shaderError.GetAddressOf()), nullptr);

	if (SUCCEEDED(hr) && shaderError != nullptr && shaderError->GetStringLength() != 0) {
		Log(shaderError->GetStringPointer());
		assert(false); // エラーはダメ絶対
	}
}

ComPtr<IDxcBlob> ShaderCompiler::GetCompileResult(const std::wstring& filePath,
												  const wchar_t* profile) {
	//========================================================================
	//	コンパイル結果（実行用バイナリ部分）を取得
	//========================================================================
	ComPtr<IDxcBlob> shaderBlob = nullptr;
	HRESULT hr = shaderResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(shaderBlob.GetAddressOf()), nullptr);
	assert(SUCCEEDED(hr));

	//========================================================================
	//	成功ログ
	//========================================================================
	Log(ConvertString(std::format(L"Compile Succeeded, path: {}, profile: {}\n", filePath, profile)));

	//========================================================================
	//	返却
	//========================================================================
	return shaderBlob;
}

ShaderCompiler::~ShaderCompiler() {}

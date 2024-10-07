#pragma once
#include <cassert>
#include <dxgi1_6.h>
#include <d3d12.h>
#include <dxcapi.h>
#include <string>
#include <wrl.h>

#pragma comment(lib,"dxcompiler.lib")
using Microsoft::WRL::ComPtr;

class ShaderCompiler{
private:

public:
	// ComPtr 型のゲッター
	ComPtr<IDxcUtils> GetDxcUtils() const{ return dxcUtils; }
	ComPtr<IDxcCompiler3> GetDxcCompiler() const{ return dxcCompiler; }
	ComPtr<IDxcIncludeHandler> GetIncludeHandler() const{ return includeHandle; }

public://メンバ関数
	ShaderCompiler();
	~ShaderCompiler();

	void InitializeDXC();

	void LoadHLSL(const std::wstring& filePath, const wchar_t* profile);

	ComPtr<IDxcBlob> CompileShader(
		// CompilerするShaderファイルへのパス
		const std::wstring& filePath,
		// Compilerに使用するProfile
		const wchar_t* profile,
		// 初期化で生成したものを3つ
		ComPtr<IDxcUtils> dxcUtils,
		ComPtr<IDxcCompiler3> dxcompiler,
		ComPtr<IDxcIncludeHandler> includeHandler
	);
	void Compile(const std::wstring& filePath,
				 const wchar_t* profile,
				 ComPtr<IDxcCompiler3> dxcompiler,
				 ComPtr<IDxcIncludeHandler> includeHandler);
	void CheckNoError();

	ComPtr<IDxcBlob> GetCompileResult(const std::wstring& filePath,
									  const wchar_t* profile);

private://メンバ変数
	ComPtr<IDxcUtils> dxcUtils = nullptr;
	ComPtr<IDxcCompiler3> dxcCompiler = nullptr;
	ComPtr<IDxcResult> shaderResult = nullptr;
	ComPtr<IDxcIncludeHandler> includeHandle = nullptr;
	DxcBuffer shaderSourceBuffer {};
	ComPtr<IDxcBlobEncoding> shaderSource = nullptr;
};

#pragma once
#include<cassert>
#include <dxgi1_6.h>
#include <d3d12.h>
#include<dxcapi.h>
#include<string>

#pragma comment(lib,"dxcompiler.lib")


class ShaderCompiler {
private:
	
public:

	IDxcUtils* GetDxcUtils()const { return dxcUtils; }
	IDxcCompiler3* GetDxcCompiler()const { return dxcCompiler; }
	IDxcIncludeHandler* GetIncludeHandler()const { return includeHandle; }

public://メンバ関数
	ShaderCompiler();
	~ShaderCompiler();
	
	void InitializeDXC();
	
	void LoadHLSL(const std::wstring& filePath, const wchar_t* profile);
	
	IDxcBlob* CompileShader(
		//CompilerするShaderファイルへのパス
		const std::wstring& filePath,
		//Compilerに使用するProfile
		const wchar_t* profile,
		//初期化で生成したものを3つ
		IDxcUtils* dxcUtils,
		IDxcCompiler3* dxcompiler,
		IDxcIncludeHandler* includeHandler
	);
	void Compile(const std::wstring& filePath,
				 const wchar_t* profile,
				 IDxcCompiler3* dxcompiler,
				 IDxcIncludeHandler* includeHandler);
	void CheckNoError();

	IDxcBlob* GetCompileResult(const std::wstring& filePath,
							   const wchar_t* profile);


private://メンバ変数
	IDxcUtils* dxcUtils = nullptr;
	IDxcCompiler3* dxcCompiler = nullptr;
	IDxcResult* shaderResult = nullptr;
	IDxcIncludeHandler* includeHandle = nullptr;
	DxcBuffer shaderSourceBuffer;
	IDxcBlobEncoding* shaderSource = nullptr;
};



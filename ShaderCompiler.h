#pragma once
#include<cassert>
#include <dxgi1_6.h>
#include <d3d12.h>
#include<dxcapi.h>
#include<string>

#pragma comment(lib,"dxcompiler.lib")


class ShaderCompiler {
private:
	// プライベートなコンストラクタ
	ShaderCompiler() {}

	// コピーと代入のオペレータを削除
	ShaderCompiler(const ShaderCompiler&) = delete;
	ShaderCompiler& operator=(const ShaderCompiler&) = delete;



public:
	// デストラクタ
	~ShaderCompiler() = default;

	// シングルトンインスタンスを取得する静的メソッド
	static ShaderCompiler* GetInstance();

public://メンバ関数
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

	IDxcBlob* GetCompileResult();

private://メンバ変数
	IDxcUtils* dxcUtils = nullptr;
	IDxcCompiler3* dxcCompiler = nullptr;
	IDxcResult* shaderResult = nullptr;
	IDxcIncludeHandler* includeHandle = nullptr;
	DxcBuffer shaderSourceBuffer;
	IDxcBlobEncoding* shaderSource = nullptr;
};



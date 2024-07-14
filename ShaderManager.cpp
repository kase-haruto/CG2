#include "ShaderManager.h"
#include"ConvertString.h"

#include<format>

void ShaderManager::InitializeDXC(){
    // DXC Compilerを初期化
    HRESULT hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils));
    assert(SUCCEEDED(hr));
    hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler));
    assert(SUCCEEDED(hr));

    // Include handlerを設定
    hr = dxcUtils->CreateDefaultIncludeHandler(&includeHandler);
    assert(SUCCEEDED(hr));
}

IDxcBlob* ShaderManager::CompileShader(const std::wstring& filePath, const wchar_t* profile){
    // HLSLファイルの読み込み
    Log(ConvertString(std::format(L"Begin CompileShader, path: {}, profile: {}\n", filePath, profile)));
    ComPtr<IDxcBlobEncoding> shaderSource;
    HRESULT hr = dxcUtils->LoadFile(filePath.c_str(), nullptr, &shaderSource);
    assert(SUCCEEDED(hr));

    DxcBuffer shaderSourceBuffer;
    shaderSourceBuffer.Ptr = shaderSource->GetBufferPointer();
    shaderSourceBuffer.Size = shaderSource->GetBufferSize();
    shaderSourceBuffer.Encoding = DXC_CP_UTF8;

    // コンパイル
    LPCWSTR arguments[] = {
        filePath.c_str(),  // コンパイル対象のHLSLファイル名
        L"-E", L"main",    // エントリーポイント
        L"-T", profile,    // シェーダープロファイル
        L"-Zi", L"-Qembed_debug",  // デバッグ情報
        L"-Od",           // 最適化無効
        L"-Zpr",          // メモリレイアウトを行優先
    };

    ComPtr<IDxcResult> shaderResult;
    hr = dxcCompiler->Compile(
        &shaderSourceBuffer,
        arguments,
        _countof(arguments),
        includeHandler.Get(),
        IID_PPV_ARGS(&shaderResult)
    );
    assert(SUCCEEDED(hr));

    // エラーチェック
    ComPtr<IDxcBlobUtf8> shaderError;
    hr = shaderResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&shaderError), nullptr);
    if (SUCCEEDED(hr) && shaderError && shaderError->GetStringLength() != 0){
        Log(shaderError->GetStringPointer());
        assert(false); // エラーがあった場合は停止
    }

    // コンパイル結果を取得
    ComPtr<IDxcBlob> shaderBlob;
    hr = shaderResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderBlob), nullptr);
    assert(SUCCEEDED(hr));

    Log(ConvertString(std::format(L"Compile Succeeded, path: {}, profile: {}\n", filePath, profile)));
    return shaderBlob.Detach();
}

bool ShaderManager::LoadShader(const std::string& name, const std::wstring& vsPath, const std::wstring& psPath){
    ComPtr<IDxcBlob> vertexShader = CompileShader(vsPath, L"vs_6_0");
    ComPtr<IDxcBlob> pixelShader = CompileShader(psPath, L"ps_6_0");

    vertexShaders[name] = vertexShader;
    pixelShaders[name] = pixelShader;
    return true;
}

const ComPtr<IDxcBlob>& ShaderManager::GetVertexShader(const std::string& name) const{
    return vertexShaders.at(name);
}

const ComPtr<IDxcBlob>& ShaderManager::GetPixelShader(const std::string& name) const{
    return pixelShaders.at(name);
}
#pragma once
#include<d3d12.h>
#include<wrl.h>
#include<string>
#include <unordered_map>
#include<dxcapi.h>
#include<cassert>


using Microsoft::WRL::ComPtr;

class ShaderManager{
public:
    ShaderManager(){}

    void InitializeDXC();
    IDxcBlob* CompileShader(const std::wstring& filePath, const wchar_t* profile);

    bool LoadShader(const std::string& name, const std::wstring& vsPath, const std::wstring& psPath);
    const ComPtr<IDxcBlob>& GetVertexShader(const std::string& name) const;
    const ComPtr<IDxcBlob>& GetPixelShader(const std::string& name) const;

private:
    std::unordered_map<std::string, ComPtr<IDxcBlob>> vertexShaders;
    std::unordered_map<std::string, ComPtr<IDxcBlob>> pixelShaders;

    // DXC関連のメンバ変数
    ComPtr<IDxcUtils> dxcUtils;
    ComPtr<IDxcCompiler3> dxcCompiler;
    ComPtr<IDxcIncludeHandler> includeHandler;
};


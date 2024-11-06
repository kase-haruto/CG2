#pragma once

//* c++
#include <d3d12.h>
#include <wrl.h>
#include <cstdint>

// ���b�V��
struct Mesh{
    template<class T> using ComPtr = ComPtr<T>;

    // ���_�o�b�t�@
    ComPtr<ID3D12Resource> vertBuff;
    // �C���f�b�N�X�o�b�t�@
    ComPtr<ID3D12Resource> indexBuff;
    // ���_�o�b�t�@�r���[
    D3D12_VERTEX_BUFFER_VIEW vbView {};
    // �C���f�b�N�X�o�b�t�@�r���[
    D3D12_INDEX_BUFFER_VIEW ibView {};
    // ���_�o�b�t�@�}�b�v
    VertexPosColor* vertMap = nullptr;
    // �C���f�b�N�X�o�b�t�@�}�b�v
    uint16_t* indexMap = nullptr;
};
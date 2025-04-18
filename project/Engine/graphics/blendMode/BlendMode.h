#pragma once

// lib
#include <d3d12.h>
#include <d3dx12.h>
#include <cassert>

enum class BlendMode{
    NONE,				//< ブレンドなし
    ALPHA,				//< アルファブレンド
    ADD,				//< 加算ブレンド
    SUB,				//< 減算ブレンド
    MUL,				//< 乗算ブレンド
    NORMAL,				//< 通常ブレンド
    SCREEN,				//< スクリーンブレンド
    kBlendModeCount		//< ブレンドモードの数
};

inline D3D12_BLEND_DESC CreateBlendDesc(BlendMode mode){
    // D3D12_DEFAULTをベースに初期化
    D3D12_BLEND_DESC blendDesc = CD3DX12_BLEND_DESC(D3D12_DEFAULT);

    // 今回はRT[0]のみ設定する想定
    auto& rt0 = blendDesc.RenderTarget[0];
    rt0.BlendEnable = FALSE;
    // 書き込みマスク(RGBAすべてに書き込み)
    rt0.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

    switch (mode){
        //-------------------------------------------------------------------------
        case BlendMode::NONE:
            // 不透明(ブレンド無効)
            // BlendEnable = FALSE のまま
            break;

            //-------------------------------------------------------------------------
        case BlendMode::ALPHA:
            // アルファブレンド
            // (SrcAlpha * Src) + (1 - SrcAlpha) * Dst
            rt0.BlendEnable = TRUE;
            rt0.SrcBlend = D3D12_BLEND_SRC_ALPHA;
            rt0.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
            rt0.BlendOp = D3D12_BLEND_OP_ADD;
            // アルファの合成は上書き
            rt0.SrcBlendAlpha = D3D12_BLEND_ZERO;
            rt0.DestBlendAlpha = D3D12_BLEND_ONE;
            rt0.BlendOpAlpha = D3D12_BLEND_OP_ADD;
            break;

            //-------------------------------------------------------------------------
        case BlendMode::ADD:
            // 加算ブレンド
            // (SrcAlpha * Src) + Dst
            // もしくは (Src * ONE) + Dst など要件に応じて
            rt0.BlendEnable = TRUE;
            rt0.SrcBlend = D3D12_BLEND_SRC_ALPHA;  // または D3D12_BLEND_ONE
            rt0.DestBlend = D3D12_BLEND_ONE;
            rt0.BlendOp = D3D12_BLEND_OP_ADD;
            // アルファは「上書き」にする例
            rt0.SrcBlendAlpha = D3D12_BLEND_ZERO;
            rt0.DestBlendAlpha = D3D12_BLEND_ONE;
            rt0.BlendOpAlpha = D3D12_BLEND_OP_ADD;
            break;

            //-------------------------------------------------------------------------
        case BlendMode::SUB:
            // 減算ブレンド (Dst - Src)
            // ここでは (1 * Dst) - (1 * Src) として実装
            rt0.BlendEnable = TRUE;
            rt0.SrcBlend = D3D12_BLEND_ONE;
            rt0.DestBlend = D3D12_BLEND_ONE;
            // 減算: BlendOp = SUBTRACT の場合は (Src - Dst)
            //        REV_SUBTRACT で (Dst - Src)
            rt0.BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;
            // アルファは上書き
            rt0.SrcBlendAlpha = D3D12_BLEND_ZERO;
            rt0.DestBlendAlpha = D3D12_BLEND_ONE;
            rt0.BlendOpAlpha = D3D12_BLEND_OP_ADD;
            break;

            //-------------------------------------------------------------------------
        case BlendMode::MUL:
            // 乗算ブレンド (Dst * Src)
            // (SrcBlend=DEST_COLOR, DestBlend=ZERO) → Result = Src * Dst
            rt0.BlendEnable = TRUE;
            rt0.SrcBlend = D3D12_BLEND_DEST_COLOR;
            rt0.DestBlend = D3D12_BLEND_ZERO;
            rt0.BlendOp = D3D12_BLEND_OP_ADD;
            // アルファは上書き
            rt0.SrcBlendAlpha = D3D12_BLEND_ZERO;
            rt0.DestBlendAlpha = D3D12_BLEND_ONE;
            rt0.BlendOpAlpha = D3D12_BLEND_OP_ADD;
            break;

            //-------------------------------------------------------------------------
        case BlendMode::NORMAL:
            // 通常ブレンド(ここでは ALPHA と同じ例)
            // あるいは「100%ソースを描く(不透明)」にしたいなら NONEと同様にするなど
            rt0.BlendEnable = TRUE;
            rt0.SrcBlend = D3D12_BLEND_SRC_ALPHA;
            rt0.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
            rt0.BlendOp = D3D12_BLEND_OP_ADD;
            rt0.SrcBlendAlpha = D3D12_BLEND_ZERO;
            rt0.DestBlendAlpha = D3D12_BLEND_ONE;
            rt0.BlendOpAlpha = D3D12_BLEND_OP_ADD;
            break;

            //-------------------------------------------------------------------------
        case BlendMode::SCREEN:
            // スクリーン近似
            // 公式には (1 - (1 - Src) * (1 - Dst)) だが固定機能だけでは完全再現が難しい
            // ここでは「SrcBlend=INV_DEST_COLOR, DestBlend=ONE」で近似
            rt0.BlendEnable = TRUE;
            rt0.SrcBlend = D3D12_BLEND_INV_DEST_COLOR; // (1 - DstColor) * Src
            rt0.DestBlend = D3D12_BLEND_ONE;            // + Dst
            rt0.BlendOp = D3D12_BLEND_OP_ADD;
            // アルファブレンドは ALPHA と同じ
            rt0.SrcBlendAlpha = D3D12_BLEND_SRC_ALPHA;
            rt0.DestBlendAlpha = D3D12_BLEND_INV_SRC_ALPHA;
            rt0.BlendOpAlpha = D3D12_BLEND_OP_ADD;
            break;

            //-------------------------------------------------------------------------
        default:
            assert(false && "Unsupported BlendMode!");
            break;
    }

    return blendDesc;
}

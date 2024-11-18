#pragma pack_matrix(row_major)

// ワールド変換の構造体：VSとPSで共有
struct TransformationMatrix{
    float4x4 WVP; // ワールド、ビュー、プロジェクション変換
    float4x4 world; // ワールド変換行列
    float4x4 worldInverseTranspose; // 法線変換用の逆転置行列
};

// 頂点シェーダーからピクセルシェーダーへのやり取りに使用する構造体
struct VSOutput{
    float4 svpos : SV_POSITION; // システム用頂点座標
    float4 color : COLOR; // 色(RGBA)
};

struct VSOutput{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD0;
};

// ワールド変換の構造体：VSとPSで共有
struct TransformationMatrix{
    float4x4 WVP; // ワールド、ビュー、プロジェクション変換
    float4x4 world; // ワールド変換行列
    float4x4 worldInverseTranspose; // 法線変換用の逆転置行列
};
// ワールド変換の構造体：VSとPSで共有
struct TransformationMatrix{
    float4x4 WVP;                   // ワールド、ビュー、プロジェクション変換
    float4x4 world;                 // ワールド変換行列
    float4x4 worldInverseTranspose; // 法線変換用の逆転置行列
};

// Vertex Shader の入力用構造体
struct VertexShaderInput{
    float4 position : POSITION0;
    float2 texcoord : TEXCOORD0;
    float3 normal :   NORMAL0;
};

// Vertex Shader の出力用構造体（Pixel Shader の入力）
struct VertexShaderOutput{
    float4 position :      SV_POSITION;
    float2 texcoord :      TEXCOORD0;
    float3 normal :        NORMAL0;
    float3 worldPosition : POSITION0;
};

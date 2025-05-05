#include "Object3d.hlsli"

/////////////////////////////////////////////////////////////////////////
//                      Material
/////////////////////////////////////////////////////////////////////////
struct Material{
    float4 color;         // 色
    int enableLighting;   // ライティングの種類
    float4x4 uvTransform; // UV座標の変換行列
    float shiniess;      // 光沢度
};

/////////////////////////////////////////////////////////////////////////
//                      camera
/////////////////////////////////////////////////////////////////////////
struct Camera{
    float3 worldPosition; // カメラのワールド座標
};

/////////////////////////////////////////////////////////////////////////
//                      DirectionalLight
/////////////////////////////////////////////////////////////////////////
struct DirectionalLight{
    float4 color;       // ライトの色
    float3 direction;   // ライトの方向
    float intensity;    // 光度
};

/////////////////////////////////////////////////////////////////////////
//                      PointLight
/////////////////////////////////////////////////////////////////////////
struct PointLight{
    float4 color;       // ライトの色
    float3 position;    // ライトの位置
    float intensity;    // 光度
    float radius;       // ライトの届く最大距離
    float decay;        // 減衰率
};

/////////////////////////////////////////////////////////////////////////
//                     Fogeffect
/////////////////////////////////////////////////////////////////////////
struct Fog{
    float start;    //開始位置
    float end;      //終了位置
    float4 color;   //色
};

// マテリアル
cbuffer MaterialConstants : register(b0){
    Material gMaterial;
}

// カメラ
cbuffer CameraConstants : register(b3){
    Camera gCamera;
}

// ディレクショナルライト
cbuffer DirectionalLightConstants : register(b2){
    DirectionalLight gDirectionalLight;
}

// ポイントライト
cbuffer PointLightConstants : register(b4){
    PointLight gPointLight;
}

// フォグ情報
cbuffer FogConstants : register(b5){
    float fogStart;
    float fogEnd;
    float4 fogColor;
}

// テクスチャとサンプラー
Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

struct PixelShaderOutput{
    float4 color : SV_TARGET0;
};


///////////////////////////////////////////////////////////////////////////////
/*                             main                                          */
//////////////////////////////////////////////////////////////////////////////

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;

    // UV座標を変換
    float4 transformedUV = mul(float4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransform);
    float4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);
    
    float3 normal = normalize(input.normal);
    float3 toEye = normalize(gCamera.worldPosition - input.worldPosition);

    ////////////////////////////////////////////////////////
    // Directional Light
    ////////////////////////////////////////////////////////
    float3 directionalDiffuse = float3(0.0f, 0.0f, 0.0f);
    float3 directionalSpecular = float3(0.0f, 0.0f, 0.0f);

    if (gMaterial.enableLighting == 0)
    {
        float NdotL = saturate(dot(normal, -gDirectionalLight.direction));
        float halfLambert = pow(NdotL * 0.5f + 0.5f, 2.0f);
        directionalDiffuse = textureColor.rgb * gDirectionalLight.color.rgb * halfLambert * gDirectionalLight.intensity;

        float3 halfVec = normalize(-gDirectionalLight.direction + toEye);
        float NdotH = dot(normal, halfVec);
        directionalSpecular = gDirectionalLight.color.rgb * pow(saturate(NdotH), gMaterial.shiniess) * gDirectionalLight.intensity;
    }
    else if (gMaterial.enableLighting == 1)
    {
        float NdotL = saturate(dot(normal, -gDirectionalLight.direction));
        directionalDiffuse = gMaterial.color.rgb * textureColor.rgb * gDirectionalLight.color.rgb * NdotL * gDirectionalLight.intensity;
    }

    ////////////////////////////////////////////////////////
    // Point Light
    ////////////////////////////////////////////////////////
    float3 pointDiffuse = float3(0.0f, 0.0f, 0.0f);
    float3 pointSpecular = float3(0.0f, 0.0f, 0.0f);

    float3 lightDir = normalize(input.worldPosition - gPointLight.position);
    float distance = length(gPointLight.position - input.worldPosition);
    float attenuation = pow(saturate(1.0f - distance / gPointLight.radius), gPointLight.decay);

    float NdotL = saturate(dot(normal, -lightDir));
    pointDiffuse = gMaterial.color.rgb * gPointLight.color.rgb * textureColor.rgb * NdotL * gPointLight.intensity * attenuation;

    float3 halfVec = normalize(-lightDir + toEye);
    float NdotH = dot(normal, halfVec);
    pointSpecular = gPointLight.color.rgb * pow(saturate(NdotH), gMaterial.shiniess) * gPointLight.intensity * attenuation;

    ////////////////////////////////////////////////////////
    // 合成 + トーンマッピング + ガンマ補正
    ////////////////////////////////////////////////////////
    float3 litColor = directionalDiffuse + directionalSpecular + pointDiffuse + pointSpecular;

    // Exposure付きReinhardトーンマッピング
    float exposure = 1.0f;
    float3 toneMapped = litColor * exposure / (litColor * exposure + 1.0f);

    // ガンマ補正
    float3 gammaCorrected = pow(toneMapped, 1.0 / 2.2);

    ////////////////////////////////////////////////////////
    // フォグ
    ////////////////////////////////////////////////////////
    /*
    float fogFactor = saturate((distance - fogStart) / (fogEnd - fogStart));
    float3 finalColor = lerp(gammaCorrected, fogColor.rgb, fogFactor);
    */
    float3 finalColor = gammaCorrected;

    ////////////////////////////////////////////////////////
    // アルファ合成 + 出力
    ////////////////////////////////////////////////////////
    float alpha = gMaterial.color.a * textureColor.a;

    if (alpha <= 0.01f)
        discard;

    output.color = float4(finalColor, alpha);
    return output;
}

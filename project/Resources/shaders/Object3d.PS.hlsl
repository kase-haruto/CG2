#include "Object3D.hlsli"

/////////////////////////////////////////////////////////////////////////
//                      Material
/////////////////////////////////////////////////////////////////////////
struct Material {
    float4 color;
    int enableLighting;
    float4x4 uvTransform;
    float shiniess;
};

/////////////////////////////////////////////////////////////////////////
//                      DirectionalLight
/////////////////////////////////////////////////////////////////////////
struct DirectionalLight {
    float4 color;
    float3 direction;
    float intensity;
};

/////////////////////////////////////////////////////////////////////////
//                      PointLight
/////////////////////////////////////////////////////////////////////////
struct PointLight {
    float4 color;
    float3 position;
    float intensity;
    float radius;
    float decay;
};


// マテリアル
cbuffer MaterialConstants : register(b0) {
    Material gMaterial;
}

// ディレクショナルライト
cbuffer DirectionalLightConstants : register(b2) {
    DirectionalLight gDirectionalLight;
}

// ポイントライト
cbuffer PointLightConstants : register(b4) {
    PointLight gPointLight;
}

/////////////////////////////////////////////////////////////////////////
//                      環境マップ
/////////////////////////////////////////////////////////////////////////
TextureCube<float4> gEnvironmentMap : register(t1);
// テクスチャとサンプラー
Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);


struct PixelShaderOutput {
    float4 color : SV_TARGET0;
};

///////////////////////////////////////////////////////////////////////////////
//                             main
///////////////////////////////////////////////////////////////////////////////

PixelShaderOutput main(VertexShaderOutput input){
    PixelShaderOutput output;

    // UV変換
    float4 transformedUV = mul(float4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransform);
    float4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);

    float3 normal = normalize(input.normal);
    float3 toEye = normalize(cameraPosition - input.worldPosition);

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

    float exposure = 1.0f;
    float3 toneMapped = litColor * exposure / (litColor * exposure + 1.0f);
    float3 gammaCorrected = pow(toneMapped, 1.0 / 2.2);

    float3 finalColor = gammaCorrected;
    
    ////////////////////////////////////////////////////////
    // 環境マップ
    ////////////////////////////////////////////////////////
    float3 cameraToPosition = normalize(input.worldPosition - cameraPosition);
    float3 reflectedVector = reflect(cameraToPosition, normalize(input.normal));
    float4 enviromentColor = gEnvironmentMap.Sample(gSampler, reflectedVector);
    
    finalColor.rgb += enviromentColor.rgb;    
    
    ////////////////////////////////////////////////////////
    // アルファ合成 + 出力
    ////////////////////////////////////////////////////////
    float alpha = gMaterial.color.a * textureColor.a;

    if (alpha <= 0.01f)
        discard;

    output.color = float4(finalColor, alpha);
    return output;
}

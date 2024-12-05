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

PixelShaderOutput main(VertexShaderOutput input){
    PixelShaderOutput output;

    // UV座標を変換
    float4 transformedUV = mul(float4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransform);

    // テクスチャサンプルを取得
    float4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);
    
    float3 normal = normalize(input.normal);

    ////////////////////////////////////////////////////////////////////
    //          DirectionalLight
    ////////////////////////////////////////////////////////////////////
    // 事前に必要な変数を宣言
    float3 toEye = normalize(gCamera.worldPosition - input.worldPosition);
    float NdotL = 0.0f;
    float halfLambertTerm = 0.0f;
    float lambertTerm = 0.0f;
    
    float3 directionalHalfVector = normalize(-gDirectionalLight.direction + toEye);
    float directionalNdotH = dot(normal, directionalHalfVector);
    float DirectionalSpecularPow = pow(saturate(directionalNdotH), gMaterial.shiniess);
    float3 directionalDiffuse = float3(0.0f, 0.0f, 0.0f);
    float3 directionalSpecular = float3(0.0f, 0.0f, 0.0f);

    // ライトの設定: Half-Lambert, Lambert, or No Lighting
    if (gMaterial.enableLighting == 0) {
        // Half-Lambert shading
        NdotL = saturate(dot(normal, -gDirectionalLight.direction));
        halfLambertTerm = pow(NdotL * 0.5f + 0.5f, 2.0f);
        directionalDiffuse = textureColor.rgb * gDirectionalLight.color.rgb * halfLambertTerm * gDirectionalLight.intensity;
        directionalSpecular = gDirectionalLight.color.rgb * gDirectionalLight.intensity * DirectionalSpecularPow * float3(1.0f, 1.0f, 1.0f);
    }
    else if (gMaterial.enableLighting == 1) {
        // Lambert shading
        NdotL = saturate(dot(normal, -gDirectionalLight.direction));
        directionalDiffuse = gMaterial.color.rgb * textureColor.rgb * gDirectionalLight.color.rgb * NdotL * gDirectionalLight.intensity;
    }else{
        // ライティングなし
        output.color = gMaterial.color * textureColor;
    }

    ////////////////////////////////////////////////////////////////////
    //          PointLight
    ////////////////////////////////////////////////////////////////////

    // ライトの方向を正規化
    float3 pointLightDir = normalize(input.worldPosition - gPointLight.position);

      // 距離減衰の計算
    float distance = length(gPointLight.position - input.worldPosition);
    float attenuation = pow(saturate(1.0f - distance / gPointLight.radius), gPointLight.decay);
  
    // 拡散反射の計算
    float pointLightNdotL = saturate(dot(normal, -pointLightDir));
    float3 pointLightDiffuse = gMaterial.color.rgb * gPointLight.color.rgb * textureColor.rgb * pointLightNdotL * gPointLight.intensity * attenuation;

    
    // 鏡面反射の計算
    float3 pointLightHalfVector = normalize(-pointLightDir + toEye);
    float pointLightNDotH = dot(normal, pointLightHalfVector);
    float pointLightSpecularPow = pow(saturate(pointLightNDotH), gMaterial.shiniess); // 'shiniess' を 'shininess' に修正
    float3 pointLightSpecular = gPointLight.color.rgb * pointLightSpecularPow * gPointLight.intensity * attenuation;
    
    // 拡散反射と鏡面反射を既存の出力カラーに追加
    float3 diffuse = directionalDiffuse + pointLightDiffuse;
    float3 specular = directionalSpecular + pointLightSpecular;
    
    
    ////////////////////////////////////////////////////////////////////
    //          fog
    ////////////////////////////////////////////////////////////////////
    float4 baseColor;
    baseColor.rgb = diffuse + specular;
    // トーンマッピング
    baseColor.rgb = baseColor.rgb / (baseColor.rgb + float3(1.0, 1.0, 1.0));

    // ガンマ補正
    output.color.rgb = pow(baseColor.rgb, 1.0 / 2.2);
    baseColor.a = gMaterial.color.a * textureColor.a;
    
    //カメラからの距離
    float distanceToCamera = length(input.worldPosition.xyz - gCamera.worldPosition);
    
    //フォグの密度を計算(霧の中では0外では1)
    float fogFactor = saturate((distanceToCamera - fogStart) / (fogEnd - fogStart));
    
    //フォグの色をブレンド
    float4 foggedColor = lerp(baseColor, fogColor, fogFactor);
    
    output.color = gMaterial.color * foggedColor;
    
    // アルファ値が0の場合、ピクセルを破棄
    if (output.color.a == 0.0){
        discard;
    }

    return output;
}

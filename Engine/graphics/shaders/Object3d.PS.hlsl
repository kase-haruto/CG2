#include "Object3d.hlsli"

Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);
ConstantBuffer<Material> gMaterial : register(b0);
ConstantBuffer<DirectionalLight> gDirectionalLight : register(b2);
ConstantBuffer<Camera> gCamera : register(b3);
ConstantBuffer<PointLight> gPointLight : register(b4);

PixelShaderOutput main(VertexShaderOutput input){
    PixelShaderOutput output;

    // UV座標を変換
    float4 transformedUV = mul(float4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransform);

    // テクスチャサンプルを取得
    float4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);


    ////////////////////////////////////////////////////////////////////
    //          DirectionalLight
    ////////////////////////////////////////////////////////////////////
    // 事前に必要な変数を宣言
    float NdotL = 0.0f;
    float halfLambertTerm = 0.0f;
    float lambertTerm = 0.0f;
    float3 diffuse = float3(0.0f, 0.0f, 0.0f);
    float3 specular = float3(0.0f, 0.0f, 0.0f);

    /*-----------------------------
    ライトの設定
    0.halfLambert
    1.lambert
    2.specular/diffuse
    3~.noLighting
    -----------------------------*/
    switch (gMaterial.enableLighting){
        case 0:
            // Half-Lambert shading
            NdotL = saturate(dot(normalize(input.normal), -gDirectionalLight.direction));
            halfLambertTerm = pow(NdotL * 0.5f + 0.5f, 2.0f);
            output.color.rgb = gMaterial.color.rgb * textureColor.rgb * gDirectionalLight.color.rgb * halfLambertTerm * gDirectionalLight.intensity;
            output.color.a = gMaterial.color.a * textureColor.a;
            break;

        case 1:
            // Lambert shading
            NdotL = saturate(dot(normalize(input.normal), -gDirectionalLight.direction));
            output.color.rgb = gMaterial.color.rgb * textureColor.rgb * gDirectionalLight.color.rgb * NdotL * gDirectionalLight.intensity;
            output.color.a = gMaterial.color.a * textureColor.a;
            break;

        case 2:
        {
            // スペキュラハイライトと拡散反射を計算
            float3 toEye = normalize(gCamera.worldPosition - input.worldPosition);
            float3 halfVector = normalize(-gDirectionalLight.direction + toEye);
            float NDotH = dot(normalize(input.normal), halfVector);

            // 鏡面反射の強さを計算
            float specularPow = pow(saturate(NDotH), gMaterial.shiniess);

            // 拡散反射の計算
            NdotL = saturate(dot(normalize(input.normal), -gDirectionalLight.direction));
            diffuse = gMaterial.color.rgb * textureColor.rgb * gDirectionalLight.color.rgb * NdotL * gDirectionalLight.intensity;

            // 鏡面反射の計算
            specular = gDirectionalLight.color.rgb * gDirectionalLight.intensity * specularPow * float3(1.0f, 1.0f, 1.0f);

            // 拡散反射と鏡面反射を合計
            output.color.rgb = diffuse + specular;
            output.color.a = gMaterial.color.a * textureColor.a;
        }
        break;

        default:
            // ライティングなし
            output.color = gMaterial.color * textureColor;
            break;
    }

    ////////////////////////////////////////////////////////////////////
    //          PointLight
    ////////////////////////////////////////////////////////////////////

    //// ピクセルへのベクトルを計算
    //float3 toPixel = gPointLight.position - input.worldPosition;

    //// ライトの方向を正規化
    //float3 lightDir = normalize(toPixel);

    //// 拡散反射の計算
    //float pointLightNdotL = saturate(dot(normalize(input.normal), lightDir));
    //float3 pointLightDiffuse = gPointLight.color.rgb * textureColor.rgb * pointLightNdotL * gPointLight.intensity;

    //// 鏡面反射の計算
    //float3 toEye = normalize(gCamera.worldPosition - input.worldPosition);
    //float3 halfVector = normalize(lightDir + toEye);
    //float NDotH = dot(normalize(input.normal), halfVector);
    //float specularPow = pow(saturate(NDotH), gMaterial.shiniess);
    //float3 pointLightSpecular = gPointLight.color.rgb * gPointLight.intensity * specularPow;

    //// 拡散反射と鏡面反射を既存の出力カラーに追加
    //output.color.rgb = pointLightDiffuse + pointLightSpecular + diffuse + specular;
    //output.color.a = gMaterial.color.a * textureColor.a;

    // アルファ値が0の場合、ピクセルを破棄
    if (output.color.a == 0.0){
        discard;
    }

    return output;
}
#include "Object3D.hlsli"

///////////////////////////////////////////////////////////////////////////////
//                            structs
///////////////////////////////////////////////////////////////////////////////
// マテリアル
struct Material {
    float4 color;
    int enableLighting;
    float4x4 uvTransform;
    float shiniess;

	// enviromentMap
    bool isReflect;
    float enviromentCoefficient;
    float roughness; // 0.0（鏡のような反射）～ 1.0（完全にぼけた反射）
};

// ディレクショナルライト
struct DirectionalLight {
	float4 color;
	float3 direction;
	float intensity;
};

// ポイントライト
struct PointLight {
	float4 color;
	float3 position;
	float intensity;
	float radius;
	float decay;
};

///////////////////////////////////////////////////////////////////////////////
//                            cbuffers
///////////////////////////////////////////////////////////////////////////////
cbuffer MaterialConstants : register(b0) {
	Material gMaterial;
}
cbuffer DirectionalLightConstants : register(b2) {
	DirectionalLight gDirectionalLight;
}
cbuffer PointLightConstants : register(b4) {
	PointLight gPointLight;
}

///////////////////////////////////////////////////////////////////////////////
//                            tables
///////////////////////////////////////////////////////////////////////////////
TextureCube<float4> gEnvironmentMap : register(t1);
Texture2D<float4> gTexture : register(t0);

///////////////////////////////////////////////////////////////////////////////
//                            samplers
///////////////////////////////////////////////////////////////////////////////
SamplerState gSampler : register(s0);

///////////////////////////////////////////////////////////////////////////////
//                            出力
///////////////////////////////////////////////////////////////////////////////
struct PixelShaderOutput {
	float4 color : SV_TARGET0;
};

///////////////////////////////////////////////////////////////////////////////
//                    関数: トーンマッピング + ガンマ補正
///////////////////////////////////////////////////////////////////////////////
float3 ApplyToneMappingAndGamma(float3 color, float exposure) {
	float3 toneMapped = color * exposure / (color * exposure + 1.0f);
	return pow(toneMapped, 1.0 / 2.2);
}

///////////////////////////////////////////////////////////////////////////////
//                    関数: ディレクショナルライト
///////////////////////////////////////////////////////////////////////////////
void ComputeDirectionalLight(
    float3 normal,
    float3 toEye,
    float3 textureColor,
    out float3 diffuse,
    out float3 specular
) {
	diffuse = float3(0.0f, 0.0f, 0.0f);
	specular = float3(0.0f, 0.0f, 0.0f);

	if(gMaterial.enableLighting == 0) {
		float NdotL = saturate(dot(normal, -gDirectionalLight.direction));
		float halfLambert = pow(NdotL * 0.5f + 0.5f, 2.0f);
		diffuse = textureColor * gDirectionalLight.color.rgb * halfLambert * gDirectionalLight.intensity;

		float3 halfVec = normalize(-gDirectionalLight.direction + toEye);
		float NdotH = saturate(dot(normal, halfVec));
		specular = gDirectionalLight.color.rgb * pow(NdotH, gMaterial.shiniess) * gDirectionalLight.intensity;
	}
	else if(gMaterial.enableLighting == 1) {
		float NdotL = saturate(dot(normal, -gDirectionalLight.direction));
		diffuse = gMaterial.color.rgb * textureColor * gDirectionalLight.color.rgb * NdotL * gDirectionalLight.intensity;
	}
}

///////////////////////////////////////////////////////////////////////////////
//                    関数: ポイントライト
///////////////////////////////////////////////////////////////////////////////
void ComputePointLight(
    float3 normal,
    float3 toEye,
    float3 worldPos,
    float3 textureColor,
    out float3 diffuse,
    out float3 specular
) {
	diffuse = float3(0.0f, 0.0f, 0.0f);
	specular = float3(0.0f, 0.0f, 0.0f);

	float3 lightDir = normalize(worldPos - gPointLight.position);
	float distance = length(gPointLight.position - worldPos);
	float attenuation = pow(saturate(1.0f - distance / gPointLight.radius), gPointLight.decay);

	float NdotL = saturate(dot(normal, -lightDir));
	diffuse = gMaterial.color.rgb * gPointLight.color.rgb * textureColor * NdotL * gPointLight.intensity * attenuation;

	float3 halfVec = normalize(-lightDir + toEye);
	float NdotH = saturate(dot(normal, halfVec));
	specular = gPointLight.color.rgb * pow(NdotH, gMaterial.shiniess) * gPointLight.intensity * attenuation;
}

///////////////////////////////////////////////////////////////////////////////
//                              main
///////////////////////////////////////////////////////////////////////////////
PixelShaderOutput main(VertexShaderOutput input) {
	PixelShaderOutput output;

    //================= UV 変換 & テクスチャ取得 =================
	float4 transformedUV = mul(float4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransform);
	float4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);

	float3 normal = normalize(input.normal);
	float3 toEye = normalize(cameraPosition - input.worldPosition);

    //================= ライト計算 =================
	float3 directionalDiffuse, directionalSpecular;
	ComputeDirectionalLight(normal, toEye, textureColor.rgb, directionalDiffuse, directionalSpecular);

	float3 pointDiffuse, pointSpecular;
	ComputePointLight(normal, toEye, input.worldPosition, textureColor.rgb, pointDiffuse, pointSpecular);

    //================= 照明合成 =================
	float3 litColor = directionalDiffuse + directionalSpecular + pointDiffuse + pointSpecular;

    //================= トーンマッピング + ガンマ補正 =================
	float3 finalColor = ApplyToneMappingAndGamma(litColor, 1.0f);

    //================= 環境マップ =================
    if (gMaterial.isReflect) {
        float3 viewDir = normalize(input.worldPosition - cameraPosition);
        float3 reflectDir = reflect(viewDir, normal);

		// mipLevel を roughness に応じて指定
        const float maxMipLevel = 7.0f; // ← キューブマップに応じて調整（最大mip数）
        float mipLevel = saturate(gMaterial.roughness) * maxMipLevel;

        float3 envColor = gEnvironmentMap.SampleLevel(gSampler, reflectDir, mipLevel).rgb;
        finalColor += envColor * gMaterial.enviromentCoefficient;
    }

    //================= アルファ計算 =================
	float alpha = gMaterial.color.a * textureColor.a;
	if(alpha <= 0.01f)
		discard;

    //================= 出力 =================
	output.color = float4(finalColor, alpha);
	return output;
}

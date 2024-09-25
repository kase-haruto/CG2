#include "Object3d.hlsli"

Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);
ConstantBuffer<Material> gMaterial : register(b0);
ConstantBuffer<DirectionalLight> gDirectionalLight : register(b2);
ConstantBuffer<Camera> gCamera : register(b3);
ConstantBuffer<PointLight> gPointLight : register(b4);

PixelShaderOutput main(VertexShaderOutput input){
    PixelShaderOutput output;

    // UV���W��ϊ�
    float4 transformedUV = mul(float4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransform);

    // �e�N�X�`���T���v�����擾
    float4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);


    ////////////////////////////////////////////////////////////////////
    //          DirectionalLight
    ////////////////////////////////////////////////////////////////////
    // ���O�ɕK�v�ȕϐ���錾
    float NdotL = 0.0f;
    float halfLambertTerm = 0.0f;
    float lambertTerm = 0.0f;
    float3 diffuse = float3(0.0f, 0.0f, 0.0f);
    float3 specular = float3(0.0f, 0.0f, 0.0f);

    /*-----------------------------
    ���C�g�̐ݒ�
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
            // �X�y�L�����n�C���C�g�Ɗg�U���˂��v�Z
            float3 toEye = normalize(gCamera.worldPosition - input.worldPosition);
            float3 halfVector = normalize(-gDirectionalLight.direction + toEye);
            float NDotH = dot(normalize(input.normal), halfVector);

            // ���ʔ��˂̋������v�Z
            float specularPow = pow(saturate(NDotH), gMaterial.shiniess);

            // �g�U���˂̌v�Z
            NdotL = saturate(dot(normalize(input.normal), -gDirectionalLight.direction));
            diffuse = gMaterial.color.rgb * textureColor.rgb * gDirectionalLight.color.rgb * NdotL * gDirectionalLight.intensity;

            // ���ʔ��˂̌v�Z
            specular = gDirectionalLight.color.rgb * gDirectionalLight.intensity * specularPow * float3(1.0f, 1.0f, 1.0f);

            // �g�U���˂Ƌ��ʔ��˂����v
            output.color.rgb = diffuse + specular;
            output.color.a = gMaterial.color.a * textureColor.a;
        }
        break;

        default:
            // ���C�e�B���O�Ȃ�
            output.color = gMaterial.color * textureColor;
            break;
    }

    ////////////////////////////////////////////////////////////////////
    //          PointLight
    ////////////////////////////////////////////////////////////////////

    //// �s�N�Z���ւ̃x�N�g�����v�Z
    //float3 toPixel = gPointLight.position - input.worldPosition;

    //// ���C�g�̕����𐳋K��
    //float3 lightDir = normalize(toPixel);

    //// �g�U���˂̌v�Z
    //float pointLightNdotL = saturate(dot(normalize(input.normal), lightDir));
    //float3 pointLightDiffuse = gPointLight.color.rgb * textureColor.rgb * pointLightNdotL * gPointLight.intensity;

    //// ���ʔ��˂̌v�Z
    //float3 toEye = normalize(gCamera.worldPosition - input.worldPosition);
    //float3 halfVector = normalize(lightDir + toEye);
    //float NDotH = dot(normalize(input.normal), halfVector);
    //float specularPow = pow(saturate(NDotH), gMaterial.shiniess);
    //float3 pointLightSpecular = gPointLight.color.rgb * gPointLight.intensity * specularPow;

    //// �g�U���˂Ƌ��ʔ��˂������̏o�̓J���[�ɒǉ�
    //output.color.rgb = pointLightDiffuse + pointLightSpecular + diffuse + specular;
    //output.color.a = gMaterial.color.a * textureColor.a;

    // �A���t�@�l��0�̏ꍇ�A�s�N�Z����j��
    if (output.color.a == 0.0){
        discard;
    }

    return output;
}
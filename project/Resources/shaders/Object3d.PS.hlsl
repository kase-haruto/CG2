#include "Object3d.hlsli"

/////////////////////////////////////////////////////////////////////////
//                      Material
/////////////////////////////////////////////////////////////////////////
struct Material{
    float4 color;         // �F
    int enableLighting;   // ���C�e�B���O�̎��
    float4x4 uvTransform; // UV���W�̕ϊ��s��
    float shiniess;      // ����x
};

/////////////////////////////////////////////////////////////////////////
//                      camera
/////////////////////////////////////////////////////////////////////////
struct Camera{
    float3 worldPosition; // �J�����̃��[���h���W
};

/////////////////////////////////////////////////////////////////////////
//                      DirectionalLight
/////////////////////////////////////////////////////////////////////////
struct DirectionalLight{
    float4 color;       // ���C�g�̐F
    float3 direction;   // ���C�g�̕���
    float intensity;    // ���x
};

/////////////////////////////////////////////////////////////////////////
//                      PointLight
/////////////////////////////////////////////////////////////////////////
struct PointLight{
    float4 color;       // ���C�g�̐F
    float3 position;    // ���C�g�̈ʒu
    float intensity;    // ���x
    float radius;       // ���C�g�̓͂��ő勗��
    float decay;        // ������
};

/////////////////////////////////////////////////////////////////////////
//                     Fogeffect
/////////////////////////////////////////////////////////////////////////
struct Fog{
    float start;    //�J�n�ʒu
    float end;      //�I���ʒu
    float4 color;   //�F
};

// �}�e���A��
cbuffer MaterialConstants : register(b0){
    Material gMaterial;
}

// �J����
cbuffer CameraConstants : register(b3){
    Camera gCamera;
}

// �f�B���N�V���i�����C�g
cbuffer DirectionalLightConstants : register(b2){
    DirectionalLight gDirectionalLight;
}

// �|�C���g���C�g
cbuffer PointLightConstants : register(b4){
    PointLight gPointLight;
}

// �t�H�O���
cbuffer FogConstants : register(b5){
    float fogStart;
    float fogEnd;
    float4 fogColor;
}

// �e�N�X�`���ƃT���v���[
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

    // UV���W��ϊ�
    float4 transformedUV = mul(float4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransform);

    // �e�N�X�`���T���v�����擾
    float4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);
    
    float3 normal = normalize(input.normal);

    ////////////////////////////////////////////////////////////////////
    //          DirectionalLight
    ////////////////////////////////////////////////////////////////////
    // ���O�ɕK�v�ȕϐ���錾
    float3 toEye = normalize(gCamera.worldPosition - input.worldPosition);
    float NdotL = 0.0f;
    float halfLambertTerm = 0.0f;
    float lambertTerm = 0.0f;
    
    float3 directionalHalfVector = normalize(-gDirectionalLight.direction + toEye);
    float directionalNdotH = dot(normal, directionalHalfVector);
    float DirectionalSpecularPow = pow(saturate(directionalNdotH), gMaterial.shiniess);
    float3 directionalDiffuse = float3(0.0f, 0.0f, 0.0f);
    float3 directionalSpecular = float3(0.0f, 0.0f, 0.0f);

    // ���C�g�̐ݒ�: Half-Lambert, Lambert, or No Lighting
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
        // ���C�e�B���O�Ȃ�
        output.color = gMaterial.color * textureColor;
    }

    ////////////////////////////////////////////////////////////////////
    //          PointLight
    ////////////////////////////////////////////////////////////////////

    // ���C�g�̕����𐳋K��
    float3 pointLightDir = normalize(input.worldPosition - gPointLight.position);

      // ���������̌v�Z
    float distance = length(gPointLight.position - input.worldPosition);
    float attenuation = pow(saturate(1.0f - distance / gPointLight.radius), gPointLight.decay);
  
    // �g�U���˂̌v�Z
    float pointLightNdotL = saturate(dot(normal, -pointLightDir));
    float3 pointLightDiffuse = gMaterial.color.rgb * gPointLight.color.rgb * textureColor.rgb * pointLightNdotL * gPointLight.intensity * attenuation;

    
    // ���ʔ��˂̌v�Z
    float3 pointLightHalfVector = normalize(-pointLightDir + toEye);
    float pointLightNDotH = dot(normal, pointLightHalfVector);
    float pointLightSpecularPow = pow(saturate(pointLightNDotH), gMaterial.shiniess); // 'shiniess' �� 'shininess' �ɏC��
    float3 pointLightSpecular = gPointLight.color.rgb * pointLightSpecularPow * gPointLight.intensity * attenuation;
    
    // �g�U���˂Ƌ��ʔ��˂������̏o�̓J���[�ɒǉ�
    float3 diffuse = directionalDiffuse + pointLightDiffuse;
    float3 specular = directionalSpecular + pointLightSpecular;
    
    
    ////////////////////////////////////////////////////////////////////
    //          fog
    ////////////////////////////////////////////////////////////////////
    float4 baseColor;
    baseColor.rgb = diffuse + specular;
    // �g�[���}�b�s���O
    baseColor.rgb = baseColor.rgb / (baseColor.rgb + float3(1.0, 1.0, 1.0));

    // �K���}�␳
    output.color.rgb = pow(baseColor.rgb, 1.0 / 2.2);
    baseColor.a = gMaterial.color.a * textureColor.a;
    
    //�J��������̋���
    float distanceToCamera = length(input.worldPosition.xyz - gCamera.worldPosition);
    
    //�t�H�O�̖��x���v�Z(���̒��ł�0�O�ł�1)
    float fogFactor = saturate((distanceToCamera - fogStart) / (fogEnd - fogStart));
    
    //�t�H�O�̐F���u�����h
    float4 foggedColor = lerp(baseColor, fogColor, fogFactor);
    
    output.color = gMaterial.color * foggedColor;
    
    // �A���t�@�l��0�̏ꍇ�A�s�N�Z����j��
    if (output.color.a == 0.0){
        discard;
    }

    return output;
}

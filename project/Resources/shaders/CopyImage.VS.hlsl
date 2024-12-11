#include "CopyImage.hlsli"

static const uint kNumVertex = 3;

static const float4 kPositions[kNumVertex] ={
    { -1.0f, 1.0f, 0.0f, 1.0f },    //ç∂è„
    { 3.0f, 1.0f, 0.0f, 1.0f },     //âEè„
    { -1.0f, -3.0f, 0.0f, 1.0f },   //ç∂â∫
};

static const float2 kTexcoords[kNumVertex] ={
    { 0.0f, 0.0f }, //ç∂è„
    { 2.0f, 0.0f }, //âEè„
    { 0.0f, 2.0f }, //ç∂â∫
};

VertexShaderOutput main(uint vertexId:SV_VertexID) {
    
    VertexShaderOutput output;
    output.position = kPositions[vertexId];
    output.texcoord = kTexcoords[vertexId];
    return output;
}
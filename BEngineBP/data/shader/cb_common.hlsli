#include "cb_shader_defines.hlsli"

struct VertexInput
{
    float3 pos : POS;
    float2 uv : TEX;
    float3 norm : NORM;
    float4 boneids : BONEID;
    float4 bonew : BONEW;
};

struct VertexOutput
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD;
    float3 norm : NORMAL;
    float3 lightPos[LIGHTS_COUNT] : TEXCOORD1;
    float4 worldPos : WORLDPOS;
};

SamplerState textureSampler : register(s0);

Texture2D volume : register(t0);
Texture2D specular : register(t1);
Texture2D normal : register(t2);
Texture2D special_1 : register(t3);

struct InstancedViewBuffer_t
{
    float4x4 modelMatrix;
};

StructuredBuffer<InstancedViewBuffer_t> InstancedViewBuffers : register(t5);



cbuffer modelViewBuffer : register(b0)
{
    float4x4 perspectiveMatrix;
    float4x4 viewMatrix;
};

cbuffer animationBuffer : register(b1)
{
    float deltaTime;
    double currTime;
}

namespace Lights
{
    struct DirectionalLight
    { 
        float3 lightDirection;
        float brightness;
        float4 diffuseColor;
    };
    
    struct PointLight
    {
        float3 position;
        float brightness;
        float4 diffuseColor;
    };
}

cbuffer lightBuffer : register(b2)
{
    Lights::DirectionalLight directionalLight;
    Lights::PointLight pointLights[LIGHTS_COUNT];
}
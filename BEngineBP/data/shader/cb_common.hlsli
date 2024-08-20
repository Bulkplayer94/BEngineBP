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
};

SamplerState textureSampler : register(s0);

Texture2D volume : register(t0);
Texture2D diffuse : register(t1);
Texture2D specular : register(t2);
Texture2D normal : register(t3);
Texture2D special_1 : register(t4);

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
        float4 diffuseColor;
        float padding;
    };
    
    struct PointLight
    {
        float3 position;
        float4 diffuseColor;
        float padding;
    };
}

cbuffer lightBuffer : register(b2)
{
    Lights::DirectionalLight directionalLight;
    Lights::PointLight pointLights[LIGHTS_COUNT];
}

cbuffer boneBuffer : register(b3)
{
    float4x4 boneMatrix;
}
struct VertexInput
{
    float3 pos : POS;
    float2 uv : TEX;
    float3 norm : NORM;
};

struct VertexOutput
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD;
};

SamplerState textureSampler : register(s0);

Texture2D volume : register(t0);
Texture2D diffuse : register(t1);
Texture2D specular : register(t2);

cbuffer modelViewBuffer : register(b0)
{
    float4x4 modelViewProj;
};

cbuffer animationBuffer : register(b1)
{
    float deltaTime;
    double currTime;
};
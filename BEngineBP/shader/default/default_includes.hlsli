cbuffer constants : register(b0)
{
    float4x4 modelViewProj;
};

struct VS_Input
{
    float3 pos : POS;
    float2 uv : TEX;
};

struct VS_Output
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD;
};

Texture2D mytexture : register(t0);
SamplerState mysampler : register(s0)
{
    Filter = Anisotropic;
    MaxAnisotropy = 16;
    AddressU = Wrap;
    AddressV = Wrap;
};
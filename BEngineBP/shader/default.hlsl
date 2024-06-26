cbuffer constants : register(b0)
{
    float4x4 modelViewProj;
};

struct VS_Input
{
    float3 pos : POS;
    float2 uv : TEX;
    float3 norm : NORM;
};

struct VS_Output
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD;
};

Texture2D mytexture : register(t0);
SamplerState mysampler : register(s0);
//{
//    Filter = Anisotropic;
//    MaxAnisotropy = 16;
//    AddressU = Wrap;
//    AddressV = Wrap;
//};

VS_Output vs_main(VS_Input input)
{
    VS_Output output;
    output.pos = mul(float4(input.pos, 1.0f), modelViewProj);
    output.uv = input.uv;
    return output;
}

float4 ps_main(VS_Output input) : SV_TARGET
{
    //return float4(1.0F, 0.0F, 0.0F, 1.0F);
    return mytexture.Sample(mysampler, input.uv);
}
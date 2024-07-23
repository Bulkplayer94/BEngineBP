cbuffer constants : register(b0)
{
    float4x4 modelViewProj;
    float4x4 worldMat;
    float4x4 perspMat;
    float4x4 viewMat;
};

struct VS_INPUT
{
    float3 pos : POS;
    float2 uv : TEX;
    float3 norm : NORM;
};

struct PS_INPUT
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD;
    float3 norm : NORMAL;
    float height : HEIGHT;
};
    
PS_INPUT vs_main(VS_INPUT input)
{
    float posZ = input.pos.z;
    
    PS_INPUT output;
    float4 worldPos = mul(float4(input.pos, 1.0f), worldMat);
    float4 viewPos = mul(worldPos, viewMat);
    output.pos = mul(viewPos, perspMat);
    
    output.uv = input.uv;
    
    output.norm = mul(input.norm, (float3x3) worldMat);
    output.norm = normalize(output.norm);
    
    output.height = clamp(posZ / 100.0F, 0.0F, 1.0F);
    
    return output;
}

float4 ps_main(PS_INPUT input) : SV_Target
{
    const float4 diffusionColor = { 1.0F, 1.0F, 1.0F, 1.0F };
    const float4 waterColor = { 0.0F, 0.2F, 0.0F, 0.5F };
    
    float4 outputColor = (input.height * diffusionColor) * waterColor;
    
    return waterColor;
}
struct VSInput
{
    float2 position : POSITION;
    float2 uv : TEX;
    uint id : SV_InstanceID;
};

struct VSOutput
{
    float4 position : SV_Position;
    float2 uvCoords : TEXCOORD0;
    float4 color : COLOR0;
};

struct Particle
{
    float4x4 worldMatrix;
    float4 color;
};

StructuredBuffer<Particle> particleBuffer : register(t2);

cbuffer MatrixBuffer : register(b5)
{
    float4x4 viewProjectionMatrix;
};

VSOutput main(VSInput input)
{
    VSOutput output;
    
    output.position = mul(float4(input.position, 0.0F, 1.0F), particleBuffer[input.id].worldMatrix);
    output.position = mul(output.position, viewProjectionMatrix);
    output.uvCoords = input.uv;
    output.color = particleBuffer[input.id].color;
    
    return output;
}
struct VertexInput
{
    float2 pos : POSITION;
    float2 uv : TEX;
};

struct VertexOutput
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD0;
};

cbuffer MatrixBuffer : register(b3)
{
    float4x4 viewMatrix;
    float4x4 projMatrix;
};

struct ParticleInstance
{
    float4x4 modelMatrix;
};

StructuredBuffer<ParticleInstance> instanceBuffer : register(t1);

VertexOutput main(VertexInput input, uint id : SV_InstanceID)
{
    VertexOutput output;
    float4 worldPos = mul(float4(input.pos, 0.0F, 1.0F), instanceBuffer[id].modelMatrix);
    float4 viewPos = mul(worldPos, viewMatrix);
    output.pos = mul(viewPos, projMatrix);
    output.uv = input.uv;
    return output;
}
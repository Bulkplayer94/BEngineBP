struct VSInput
{
    float3 pos : POSITION;
    float2 tex : TEX;
};

struct VSOutput
{
    float4 position : SV_Position;
    float2 texCoord : TEXCOORD0;
    float4 color : COLOR0;
};

cbuffer MatrixBuffer : register(b3)
{
    float4x4 worldMatrix;
    float4x4 viewMatrix;
    float4x4 perspectiveMatrix;
}

VSOutput main(VSInput input)
{
    VSOutput output;
    float4 worldPos = mul(float4(input.pos, 1.0f), worldMatrix);
    float4 viewPos = mul(worldPos, viewMatrix);
    output.position = mul(viewPos, perspectiveMatrix);
    output.texCoord = input.tex;
    output.color = float4(1.0F, 0.0F, 0.0F, 1.0F);
    
    return output;
}

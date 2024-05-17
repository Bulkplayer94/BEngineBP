cbuffer ConstantBuffer : register(b0)
{
    float3x3 transformMatrix;
}

struct VS_INPUT
{
    float3 position : POSITION;
};

struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;

    // Transform the vertex position using the 3x3 matrix
    output.position = float4(mul(transformMatrix, input.position), 1.0f);
    output.color = float4(1.0F, 0.0F, 0.0F, 1.0F);

    return output;
}
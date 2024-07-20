#include "default_includes.hlsli"

VS_Output main(VS_Input input)
{
    VS_Output output;

    output.pos = mul(float4(input.pos, 1.0f), worldMatrix);
    output.pos = mul(output.pos, viewMat);
    output.pos = mul(output.pos, perspMat);

    output.uv = input.uv;

    output.norm = mul(input.norm, (float3x3)worldMatrix);

    return output;
}
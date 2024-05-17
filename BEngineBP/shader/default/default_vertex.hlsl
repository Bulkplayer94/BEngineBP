#include "default_includes.hlsli"

VS_Output main(VS_Input input)
{
    VS_Output output;
    output.pos = mul(float4(input.pos, 1.0f), modelViewProj);
    output.uv = input.uv;
    return output;
}
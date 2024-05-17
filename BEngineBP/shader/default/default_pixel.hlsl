#include "default_includes.hlsli"

float4 main(VS_Output input) : SV_TARGET
{
    return mytexture.Sample(mysampler, input.uv);
}
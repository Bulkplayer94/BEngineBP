#include "cb_common.hlsli"

VertexOutput main(VertexInput input)
{
    VertexOutput output;
    output.pos = mul(modelViewProj, float4(input.pos, 1.0F));
    output.uv = input.uv;
    
    return output;
}